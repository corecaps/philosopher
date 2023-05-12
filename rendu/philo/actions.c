/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 12:26:23 by jgarcia           #+#    #+#             */
/*   Updated: 2023/05/10 10:34:26 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* **************************************************************************
 * think action
 ************************************************************************** */

void	think(t_philo *philo)
{
	message(THINKING, get_stamp(philo->sim_start), (*philo->sim_running),
		philo);
	philo->state = EATING;
}

/* **************************************************************************
 * each philosopher need two forks to eat
 * each fork is a mutex lock
 * in case of alone philosopher, he will die after ttdie ms
 ************************************************************************** */

int	take_fork(pthread_mutex_t *first, pthread_mutex_t *last, t_philo *philo)
{
	pthread_mutex_lock(first);
	message(FORK, get_stamp(philo->sim_start), (*philo->sim_running), philo);
	if (philo->n_philo == 1)
	{
		usleep((philo->ttdie) * 1000);
		pthread_mutex_lock(philo->run_state_check);
		message(DEAD, get_stamp(philo->sim_start), (*philo->sim_running),
			philo);
		(*philo->sim_running) = 0;
		pthread_mutex_unlock(philo->run_state_check);
		pthread_mutex_unlock(first);
		return (1);
	}
	pthread_mutex_lock(last);
	message(FORK, get_stamp(philo->sim_start), (*philo->sim_running), philo);
	return (0);
}

/* **************************************************************************
 * after tteat ms, the philosopher leaves the forks
 ************************************************************************** */

void	leave_fork(pthread_mutex_t *first, pthread_mutex_t *last)
{
	pthread_mutex_unlock(first);
	pthread_mutex_unlock(last);
}

/* **************************************************************************
 * eat action
 * to avoid dead locks, even id philosophers take right fork first
 * odd id philosophers take left fork first
 * setting the correct stamp for the last meal of the philosopher
 ************************************************************************** */

void	eat(t_philo *philo)
{
	struct timeval	now;
	int				err;

	if (philo->id % 2 == 0)
		err = take_fork(philo->right_fork, philo->left_fork, philo);
	else
		err = take_fork(philo->left_fork, philo->right_fork, philo);
	if (err)
		return ;
	message(EATING, get_stamp(philo->sim_start), (*philo->sim_running), philo);
	gettimeofday(&now, NULL);
	philo->last_eat = now;
	philo->n_eat ++;
	usleep(philo->tteat * 1000);
	if (philo->id % 2 == 0)
		leave_fork(philo->left_fork, philo->right_fork);
	else
		leave_fork(philo->right_fork, philo->left_fork);
	philo->state = SLEEPING;
}

/* **************************************************************************
 * sleep action
 * here we check if the philosopher is alive
 * if the philosopher dies while sleeping, the simulation ends
 ************************************************************************** */

void	sleeping(t_philo *philo)
{
	long int	time;
	long int	ttthink;

	message(SLEEPING, get_stamp(philo->sim_start), (*philo->sim_running),
		philo);
	time = get_stamp(philo->last_eat);
	if (philo->tteat < philo->ttsleep)
		ttthink = 0;
	else
		ttthink = philo->tteat - philo->ttsleep;
	if ((time + philo->ttsleep + ttthink) > philo->ttdie)
	{
		usleep((philo->ttdie - time) * 1000);
		pthread_mutex_lock(philo->run_state_check);
		message(DEAD, get_stamp(philo->sim_start), (*philo->sim_running),
			philo);
		(*philo->sim_running) = 0;
		pthread_mutex_unlock(philo->run_state_check);
	}
	else
	{
		usleep(philo->ttsleep * 1000);
		philo->state = THINKING;
	}
}
