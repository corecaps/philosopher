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

void message(enum e_state action, long int stamp, int id, int alive)
{
	if (alive == 0)
		return ;
	if (action < THINKING || action > FORK || stamp < 0 || id < 0)
		return ;
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
	pthread_mutex_lock(philo->message);
	message(THINKING, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
	pthread_mutex_unlock(philo->message);
	philo->state = EATING;
}

void eat(t_philo *philo)
{
	int	left_philo;
	t_philo	*left_philo_ptr;
	struct timeval	now;

	left_philo = philo->id % philo->sim_params->n_philo + 1;
	left_philo_ptr = &philo->head[left_philo-1];

	pthread_mutex_lock(philo->alive_monitor);
	if (philo->sim_params->alive == 0)
	{
		pthread_mutex_unlock(philo->alive_monitor);
		return;
	}
	pthread_mutex_unlock(philo->alive_monitor);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
		pthread_mutex_lock(left_philo_ptr->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
	}
	else
	{
		pthread_mutex_lock(left_philo_ptr->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
		pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
	}

	pthread_mutex_lock(philo->alive_monitor);
	pthread_mutex_lock(philo->message);
	message(EATING, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
	pthread_mutex_unlock(philo->message);
	gettimeofday(&now,NULL);
	philo->last_eat = now;
	philo->eat_count ++;
	pthread_mutex_unlock(philo->alive_monitor);
	usleep(philo->sim_params->tteat * 1000);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_unlock(left_philo_ptr->right_fork);
		pthread_mutex_unlock(philo->right_fork);
	}
	else
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(left_philo_ptr->right_fork);
	}
	philo->state = SLEEPING;
}

void sleeping(t_philo *philo)
{
	long int	time;
	long int	ttthink;

	pthread_mutex_lock(philo->message);
	message(SLEEPING, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
	time = get_stamp(philo->last_eat);
	if (philo->sim_params->tteat < philo->sim_params->ttsleep)
		ttthink = 0;
	else
		ttthink = philo->sim_params->tteat - philo->sim_params->ttsleep;
	if ((time + philo->sim_params->ttsleep + ttthink) > philo->sim_params->ttdie)
	{
		usleep((philo->sim_params->ttdie - time) * 1000);
		message(DEAD, get_stamp(philo->sim_start), philo->id, philo->sim_params->alive);
		philo->live = 0;
		philo->sim_params->alive = 0;
	}
	else
	{
		usleep(philo->sim_params->ttsleep * 1000);
		philo->state = THINKING;
	}
	pthread_mutex_unlock(philo->message);
}

void	*philosopher(void *arg)
{
	t_philo		*philo;


	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->alive_monitor);
	while (philo->live && philo->sim_params->alive && (philo->eat_count < philo->n_eat) )
	{
		if (philo->state == THINKING && philo->live)
		{
			pthread_mutex_unlock(philo->alive_monitor);
			think(philo);
		}
		else if (philo->state == EATING && philo->live)
		{
			pthread_mutex_unlock(philo->alive_monitor);
			eat(philo);
		}
		else if (philo->state == SLEEPING && philo->live)
		{
			pthread_mutex_unlock(philo->alive_monitor);
			sleeping(philo);
		}
		pthread_mutex_lock(philo->alive_monitor);
	}
	pthread_mutex_unlock(philo->alive_monitor);
	return (NULL);
}

int	simulation(t_args *sim_params)
{
	t_philo			*philo;
	int				i;
	struct timeval	start;
	t_data			*data;

	philo = malloc(sizeof(t_philo) * sim_params->n_philo);
	data = malloc(sizeof(t_data));
	if (!philo  || !data)
		return (-1);
	i = 0;
	gettimeofday(&start,NULL);
	data->head_philo = (t_philo *)philo;
	data->sim_param = sim_params;
	while (i < sim_params->n_philo)
	{
		philo[i].live = 1;
		philo[i].id = i + 1;
		philo[i].sim_params = sim_params;
		philo[i].head = philo;
		philo[i].sim_start = start;
		philo[i].eat_count = 0;
		philo[i].n_eat = sim_params->n_eat;
		gettimeofday(&philo[i].last_eat, NULL);
		if (i % 2 == 0)
			philo[i].state = EATING;
		else
			philo[i].state = SLEEPING;
		philo[i].right_fork = malloc(sizeof(pthread_mutex_t));
		philo[i].alive_monitor = malloc(sizeof(pthread_mutex_t));
		philo[i].message = malloc(sizeof(pthread_mutex_t));
		if (!philo[i].right_fork || !philo[i].alive_monitor)
			return (-1);
		pthread_mutex_init(philo[i].right_fork, NULL);
		pthread_mutex_init(philo[i].alive_monitor, NULL);
		pthread_mutex_init(philo[i].message, NULL);
		i++;
	}
	i = 0;
	while (i ++ < sim_params->n_philo)
	{
		pthread_create(&philo[i-1].philo_id, NULL, philosopher, &philo[i-1]);
	}
	i = 0;
	while (i < sim_params->n_philo)
	{
		pthread_join(philo[i].philo_id, NULL);
		i++;
	}
	return (0);
}
