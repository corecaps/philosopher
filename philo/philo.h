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
# include <limits.h>

typedef struct s_args
{
	int	n_philo;
	int	ttdie;
	int	tteat;
	int	ttsleep;
	int	max_eat;
	int	alive;
	pthread_mutex_t	*message;
}	t_args;
typedef enum e_state
{
	THINKING,
	EATING,
	SLEEPING,
	DEAD,
	FORK
} t_state;
typedef struct s_philo
{
	pthread_t		philo_id;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*live_check;
	pthread_mutex_t	*message;
	int				id;
	int				live;
	int				eat_count;
	int				n_eat;
	int				max_eat;
	int 			ttdie;
	int				tteat;
	int				ttsleep;
	struct timeval	last_eat;
	struct timeval	sim_start;
	t_state			state;
}	t_philo;

int	ft_atoi(char *str);
int	simulation(t_args *sim_params);
#endif