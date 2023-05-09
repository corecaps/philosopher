/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:41:52 by jgarcia           #+#    #+#             */
/*   Updated: 2023/02/24 11:41:56 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philosopher(void *arg)
{
	t_philo		*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->run_state_check);
	while (philo->sim_running && (philo->n_eat < philo->max_eat) )
	{
		if (philo->state == THINKING && (*philo->sim_running))
		{
			pthread_mutex_unlock(philo->run_state_check);
			think(philo);
		}
		else if (philo->state == EATING && (*philo->sim_running))
		{
			pthread_mutex_unlock(philo->run_state_check);
			eat(philo);
		}
		else if (philo->state == SLEEPING && (*philo->sim_running))
		{
			pthread_mutex_unlock(philo->run_state_check);
			sleeping(philo);
		}
        else if ((*philo->sim_running) == 0)
        {
            pthread_mutex_unlock(philo->run_state_check);
            return (NULL);
        }
		pthread_mutex_lock(philo->run_state_check);
	}
	pthread_mutex_unlock(philo->run_state_check);
	return (NULL);
}

int init_philo(t_args *sim_params, t_philo *philo, int i, struct timeval *start) {
	philo[i].sim_running = &sim_params->running;
	philo[i].id = i + 1;
	philo[i].ttdie = sim_params->ttdie;
	philo[i].tteat = sim_params->tteat;
	philo[i].ttsleep = sim_params->ttsleep;
	philo[i].sim_start = (*start);
	philo[i].n_eat = 0;
	philo[i].max_eat = sim_params->max_eat;
	philo[i].message = sim_params->message;
	philo[i].run_state_check = sim_params->live_check;
	philo[i].n_philo = sim_params->n_philo;
	gettimeofday(&philo[i].last_eat, NULL);
	if (i % 2 == 0)
		philo[i].state = EATING;
	else
		philo[i].state = SLEEPING;
	philo[i].right_fork = malloc(sizeof(pthread_mutex_t));
	if (!philo[i].right_fork)
	{
		return (-1);
	}
	pthread_mutex_init(philo[i].right_fork, NULL);
	return (0);
}



int	simulation(t_args *sim_params)
{
	t_philo			*philo;
	int				i;
	struct timeval	start;

	philo = malloc(sizeof(t_philo) * sim_params->n_philo);
	i = 0;
	gettimeofday(&start,NULL);
	sim_params->message = malloc(sizeof(pthread_mutex_t));
    sim_params->live_check = malloc(sizeof(pthread_mutex_t));
	if (sim_params->message == NULL || sim_params->live_check == NULL)
		return (-1);
	pthread_mutex_init(sim_params->message, NULL);
    pthread_mutex_init(sim_params->live_check, NULL);
	while (i < sim_params->n_philo)
	{
		if (init_philo(sim_params, philo, i, &start))
			return (-1);
		i++;
	}
	i = 0;
	while (i < sim_params->n_philo)
	{
		if (i == 0)
			philo[i].left_fork = philo[sim_params->n_philo - 1].right_fork;
		else
			philo[i].left_fork = philo[i - 1].right_fork;
		i++;
	}
	i = 0;
	while (i ++ < sim_params->n_philo)
		pthread_create(&philo[i-1].philo_id, NULL, philosopher, &philo[i-1]);
	i = 0;
	while (i ++ < sim_params->n_philo)
		pthread_join(philo[i - 1].philo_id, NULL);

	clean(sim_params, philo);
	return (0);
}