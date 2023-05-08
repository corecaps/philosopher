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

void message(enum e_state action, long int stamp, int id, int alive, pthread_mutex_t *message)
{
	if (alive == 0)
		return ;
	if (action < THINKING || action > FORK || stamp < 0 || id < 0)
		return ;
    pthread_mutex_lock(message);
	if (action == EATING)
		printf("%ld %d is eating\n", stamp, id);
	else if (action == SLEEPING)
		printf("%ld %d is sleeping\n", stamp, id);
	else if (action == THINKING)
		printf("%ld %d is thinking\n", stamp, id);
	else if (action == DEAD)
		printf("%ld %d died\n", stamp, id);
	else if (action == FORK)
		printf("%ld %d has taken a fork\n", stamp, id);
    pthread_mutex_unlock(message);
}

long int get_stamp(struct timeval start)
{
	struct timeval	now;
	double diff;

	gettimeofday(&now,NULL);
	diff =1e6 * (double)(now.tv_sec - start.tv_sec) + (double)(now.tv_usec - start.tv_usec);
	diff /= 1000;
	return ((long int) diff);
}

void think(t_philo *philo)
{
	message(THINKING, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
	philo->state = EATING;
}

void eat(t_philo *philo)
{
	struct timeval	now;

	pthread_mutex_lock(philo->live_check);
	if ((*philo->live) == 0)
	{
		pthread_mutex_unlock(philo->live_check);
		return;
	}
	pthread_mutex_unlock(philo->live_check);

	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
		pthread_mutex_lock(philo->left_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
        if (philo->n_philo == 1)
        {
            usleep((philo->ttdie) * 1000);
            pthread_mutex_lock(philo->live_check);
            message(DEAD, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
            (*philo->live) = 0;
            pthread_mutex_unlock(philo->live_check);
            return ;
        }
        pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
	}
	message(EATING, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
	gettimeofday(&now,NULL);
	philo->last_eat = now;
	philo->n_eat ++;

	usleep(philo->tteat * 1000);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
	}
	else
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
	}
	philo->state = SLEEPING;
}

void sleeping(t_philo *philo)
{
	long int	time;
	long int	ttthink;

	message(SLEEPING, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
    time = get_stamp(philo->last_eat);
	if (philo->tteat < philo->ttsleep)
		ttthink = 0;
	else
		ttthink = philo->tteat - philo->ttsleep;
	if ((time + philo->ttsleep + ttthink) > philo->ttdie)
	{
		usleep((philo->ttdie - time) * 1000);
        pthread_mutex_lock(philo->live_check);
        message(DEAD, get_stamp(philo->sim_start), philo->id, (*philo->live),philo->message);
        (*philo->live) = 0;
		pthread_mutex_unlock(philo->live_check);
	}
	else
	{
		usleep(philo->ttsleep * 1000);
		philo->state = THINKING;
	}
}

void	*philosopher(void *arg)
{
	t_philo		*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->live_check);
	while (philo->live && (philo->n_eat < philo->max_eat) )
	{
		if (philo->state == THINKING && (*philo->live))
		{
			pthread_mutex_unlock(philo->live_check);
			think(philo);
		}
		else if (philo->state == EATING && (*philo->live))
		{
			pthread_mutex_unlock(philo->live_check);
			eat(philo);
		}
		else if (philo->state == SLEEPING && (*philo->live))
		{
			pthread_mutex_unlock(philo->live_check);
			sleeping(philo);
		}
        else if ((*philo->live) == 0)
        {
            pthread_mutex_unlock(philo->live_check);
            return (NULL);
        }
		pthread_mutex_lock(philo->live_check);
	}
	pthread_mutex_unlock(philo->live_check);
	return (NULL);
}

int init_philo(t_args *sim_params, t_philo *philo, int i, struct timeval *start) {
	philo[i].live = &sim_params->alive;
	philo[i].id = i + 1;
	philo[i].ttdie = sim_params->ttdie;
	philo[i].tteat = sim_params->tteat;
	philo[i].ttsleep = sim_params->ttsleep;
	philo[i].sim_start = (*start);
	philo[i].n_eat = 0;
	philo[i].max_eat = sim_params->max_eat;
	philo[i].message = sim_params->message;
	philo[i].live_check = sim_params->live_check;
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

void clean(t_args *sim_params, t_philo *philo, int i) {
	i = 0;
	while (i < sim_params->n_philo)
	{
		pthread_mutex_destroy(philo[i].right_fork);
		free(philo[i].right_fork);
		i++;
	}
	pthread_mutex_destroy(sim_params->message);
	pthread_mutex_destroy(sim_params->live_check);
	free(sim_params->message);
	free(sim_params->live_check);
	free(philo);
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

	clean(sim_params, philo, i);
	return (0);
}