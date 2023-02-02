//
// Created by corecaps on 31/01/23.
//

#include "philo.h"

void	*philosopher(void *arg)
{
	t_philo		*philo;
//	t_args		*sim_params;

	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->right_fork);
	printf("philo %d %ld here\n",philo->id, philo->philo_id);
	sleep(1);
	pthread_mutex_unlock(philo->right_fork);
	printf("philo %ld dies\n", philo->philo_id);
	//	sim_params = philo->sim_params;
//	while (philo->live)
//	{
//		if (philo->state == THINKING)
//			think(philo);
//		else if (philo->state == EATING)
//			eat(philo);
//		else if (philo->state == SLEEPING)
//			sleeping(philo);
//	}
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
		pthread_create(&philo[i].philo_id, NULL, philosopher, &philo[i]);
		i++;
	}
	sleep(5);
	return (0);
}