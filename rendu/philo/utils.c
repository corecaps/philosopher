/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:42:03 by jgarcia           #+#    #+#             */
/*   Updated: 2023/05/10 10:35:00 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* **************************************************************************
 * Ascii to integer conversion
 * @param str string to convert
 * @return the converted integer or -1 if the string is not a valid integer
 ************************************************************************** */

int	ft_atoi(char *str)
{
	long int	result;

	result = 0;
	if (!str)
		return (-1);
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	while (*str >= 48 && *str <= 57)
	{
		result = result * 10 + *str - 48;
		str++;
	}
	if (*str != '\0')
		return (-1);
	else
		return (result);
}

/* **************************************************************************
 * Print logging messages to stdout if the simulation is running
 * Each message is protected by a mutex to avoid mixing the output
 * @param action the action to log
 * @param stamp the timestamp of the action
 * @param running flag to stop the simulation
 ************************************************************************** */

void	message(enum e_state action, long int stamp, int running,
	t_philo *philo)
{
	if (running == 0)
		return ;
	if (action < THINKING || action > FORK || stamp < 0 || philo->id < 0)
		return ;
	pthread_mutex_lock(philo->message);
	if (action == EATING)
		printf("%ld %d is eating\n", stamp, philo->id);
	else if (action == SLEEPING)
		printf("%ld %d is sleeping\n", stamp, philo->id);
	else if (action == THINKING)
		printf("%ld %d is thinking\n", stamp, philo->id);
	else if (action == DEAD)
		printf("%ld %d died\n", stamp, philo->id);
	else if (action == FORK)
		printf("%ld %d has taken a fork\n", stamp, philo->id);
	pthread_mutex_unlock(philo->message);
}

/* **************************************************************************
 * Get the current timestamp in milliseconds
 ************************************************************************** */

long int	get_stamp(struct timeval start)
{
	struct timeval	now;
	double			diff;

	gettimeofday(&now, NULL);
	diff = 1e6 * (double)(now.tv_sec - start.tv_sec)
		+ (double)(now.tv_usec - start.tv_usec);
	diff /= 1000;
	return ((long int) diff);
}

/* **************************************************************************
 * Free the allocated memory and destroy the mutexes
 ************************************************************************** */

void	clean(t_args *sim_params, t_philo *philo)
{
	int	i;

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
