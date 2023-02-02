/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <jgarcia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/30 09:48:41 by jgarcia           #+#    #+#             */
/*   Updated: 2023/01/30 09:48:48 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <stdio.h>

typedef struct s_args
{
	int	n_philo;
	int	ttdie;
	int	tteat;
	int	ttsleep;
	int	n_eat;
}	t_args;
typedef enum e_state
{
	THINKING,
	EATING,
	SLEEPING,
	DEAD
} t_state;
typedef struct s_philo
{
	pthread_t		philo_id;
	int				id;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*message;
	pthread_mutex_t	*alive_monitor;
	int				live;
	int				eat_count;
	int				n_eat;
	struct timeval	last_eat;
	t_state			state;
}	t_philo;

int	ft_atoi(char *str);
int	simulation(t_args *sim_params);
#endif