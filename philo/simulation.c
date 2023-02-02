//
// Created by corecaps on 31/01/23.
//

#include "philo.h"

void think(t_philo *philo)
{
	printf("philo %d is thinking\n", philo->id);
	philo->state = EATING;
}

void eat(t_philo *philo)
{
	int	left_philo;
	t_philo	*left_philo_ptr;

	left_philo = philo->id % philo->sim_params->n_philo + 1;
	left_philo_ptr = &philo->head[left_philo-1];

//	printf("philo id:%d\t philo address:%p\tleft_philo id:%d\tleft philo address:%p\n", philo->id,philo, left_philo, left_philo_ptr);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(left_philo_ptr->right_fork);
	}
	else
	{
		pthread_mutex_lock(left_philo_ptr->right_fork);
		pthread_mutex_lock(philo->right_fork);
	}
	printf("philo %d is eating\n", philo->id);
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
	printf("philo %d is sleeping\n", philo->id);
	usleep(philo->sim_params->ttsleep * 1000);
	philo->state = THINKING;
}

void	*philosopher(void *arg)
{
	t_philo		*philo;
//	t_args		*sim_params;
	struct timeval	now;
	int time;
	philo = (t_philo *)arg;
//	pthread_mutex_lock(philo->right_fork);
//	printf("philo %d %ld here\n",philo->id, philo->philo_id);
//	sleep(1);
//	pthread_mutex_unlock(philo->right_fork);
//	printf("philo %ld dies\n", philo->philo_id);
	//	sim_params = philo->sim_params;
	while (philo->live)
	{
		gettimeofday(&now, NULL);
		time = (now.tv_sec - philo->last_eat.tv_sec) * 1000 + (now.tv_usec - philo->last_eat.tv_usec) / 1000;
		printf("[%d]\t",time);
		if (philo->state == THINKING)
			think(philo);
		else if (philo->state == EATING)
			eat(philo);
		else if (philo->state == SLEEPING)
			sleeping(philo);
	}
	return (NULL);
}

int	simulation(t_args *sim_params)
{
	t_philo		*philo;
	int			i;

	philo = malloc(sizeof(t_philo) * sim_params->n_philo);
	if (!philo)
		return (-1);
	i = 0;
	while (i < sim_params->n_philo)
	{
		philo[i].n_eat = 0;
		philo[i].live = 1;
		philo[i].id = i + 1;
		philo[i].sim_params = sim_params;
		philo[i].head = philo;
		gettimeofday(&philo[i].last_eat, NULL);
		philo[i].state = THINKING;
		philo[i].right_fork = malloc(sizeof(pthread_mutex_t));
		philo[i].message = malloc(sizeof(pthread_mutex_t));
		philo[i].alive_monitor = malloc(sizeof(pthread_mutex_t));
		if (!philo[i].right_fork || !philo[i].message || !philo[i].alive_monitor)
			return (-1);
		pthread_mutex_init(philo[i].right_fork, NULL);
		pthread_mutex_init(philo[i].message, NULL);
		pthread_mutex_init(philo[i].alive_monitor, NULL);
		printf("philo %d\t address:%p\n", philo[i].id, &philo[i]);
		i++;
	}
	i = 0;
	while (i < sim_params->n_philo)
	{
		pthread_create(&philo[i].philo_id, NULL, philosopher, &philo[i]);
		i++;
	}
	sleep(10);
	return (0);
}