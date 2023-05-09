//
// Created by jgarcia on 5/9/23.
//
#include "philo.h"

void think(t_philo *philo)
{
	message(THINKING, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
	philo->state = EATING;
}

void eat(t_philo *philo)
{
	struct timeval	now;

	pthread_mutex_lock(philo->run_state_check);
	if ((*philo->sim_running) == 0)
	{
		pthread_mutex_unlock(philo->run_state_check);
		return;
	}
	pthread_mutex_unlock(philo->run_state_check);

	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
		pthread_mutex_lock(philo->left_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
		if (philo->n_philo == 1)
		{
			usleep((philo->ttdie) * 1000);
			pthread_mutex_lock(philo->run_state_check);
			message(DEAD, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
			(*philo->sim_running) = 0;
			pthread_mutex_unlock(philo->run_state_check);
			return ;
		}
		pthread_mutex_lock(philo->right_fork);
		message(FORK, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
	}
	message(EATING, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
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

	message(SLEEPING, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
	time = get_stamp(philo->last_eat);
	if (philo->tteat < philo->ttsleep)
		ttthink = 0;
	else
		ttthink = philo->tteat - philo->ttsleep;
	if ((time + philo->ttsleep + ttthink) > philo->ttdie)
	{
		usleep((philo->ttdie - time) * 1000);
		pthread_mutex_lock(philo->run_state_check);
		message(DEAD, get_stamp(philo->sim_start), philo->id, (*philo->sim_running), philo->message);
		(*philo->sim_running) = 0;
		pthread_mutex_unlock(philo->run_state_check);
	}
	else
	{
		usleep(philo->ttsleep * 1000);
		philo->state = THINKING;
	}
}