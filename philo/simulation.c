/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:41:52 by jgarcia           #+#    #+#             */
/*   Updated: 2023/05/09 12:41:53 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philosopher(void *arg)
{
	t_philo		*philo;
	void		(*actions[3])(t_philo *philo);

	philo = (t_philo *)arg;
	actions[THINKING] = think;
	actions[EATING] = eat;
	actions[SLEEPING] = sleeping;
	pthread_mutex_lock(philo->run_state_check);
	while (philo->sim_running && (philo->n_eat < philo->max_eat))
	{
		if ((*philo->sim_running))
		{
			pthread_mutex_unlock(philo->run_state_check);
			actions[philo->state](philo);
		}
		else
		{
			pthread_mutex_unlock(philo->run_state_check);
			return (NULL);
		}
		pthread_mutex_lock(philo->run_state_check);
	}
	pthread_mutex_unlock(philo->run_state_check);
	return (NULL);
}

int	init_philo(t_args *params, t_philo *philo, int i, struct timeval *start)
{
	philo[i].sim_running = &params->running;
	philo[i].id = i + 1;
	philo[i].ttdie = params->ttdie;
	philo[i].tteat = params->tteat;
	philo[i].ttsleep = params->ttsleep;
	philo[i].sim_start = (*start);
	philo[i].n_eat = 0;
	philo[i].max_eat = params->max_eat;
	philo[i].message = params->message;
	philo[i].run_state_check = params->live_check;
	philo[i].n_philo = params->n_philo;
	gettimeofday(&philo[i].last_eat, NULL);
	if (i % 2 == 0)
		philo[i].state = EATING;
	else
		philo[i].state = SLEEPING;
	philo[i].right_fork = malloc(sizeof(pthread_mutex_t));
	if (!philo[i].right_fork)
		return (-1);
	pthread_mutex_init(philo[i].right_fork, NULL);
	return (0);
}

t_philo	*alloc(t_args *sim_params)
{
	t_philo	*philo;

	philo = malloc(sizeof(t_philo) * sim_params->n_philo);
	if (!philo)
		return (NULL);
	sim_params->message = malloc(sizeof(pthread_mutex_t));
	sim_params->live_check = malloc(sizeof(pthread_mutex_t));
	if (sim_params->message == NULL || sim_params->live_check == NULL)
	{
		free(philo);
		return (NULL);
	}
	pthread_mutex_init(sim_params->message, NULL);
	pthread_mutex_init(sim_params->live_check, NULL);
	return (philo);
}

void	thread_handler(t_args *sim_params, t_philo *philo)
{
	int	i;

	i = 0;
	while (i ++ < sim_params->n_philo)
		pthread_create(&philo[i - 1].philo_id, NULL, philosopher,
			&philo[i - 1]);
	i = 0;
	while (i ++ < sim_params->n_philo)
		pthread_join(philo[i - 1].philo_id, NULL);
	clean(sim_params, philo);
}

int	simulation(t_args *sim_params)
{
	t_philo			*philo;
	int				i;
	struct timeval	start;

	i = 0;
	philo = alloc(sim_params);
	if (!philo)
		return (-1);
	gettimeofday(&start, NULL);
	while (i < sim_params->n_philo)
	{
		if (init_philo(sim_params, philo, i, &start))
			return (-1);
		i++;
	}
	while (i > 0)
	{
		if (i == 1)
			philo[i - 1].left_fork = philo[sim_params->n_philo - 1].right_fork;
		else
			philo[i - 1].left_fork = philo[i - 2].right_fork;
		i --;
	}
	thread_handler(sim_params, philo);
	return (0);
}
