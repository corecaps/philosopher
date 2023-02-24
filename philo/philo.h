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
	DEAD,
	FORK
} t_state;
typedef struct s_philo
{
	t_args			*sim_params;
	struct s_philo			*head;
	pthread_t		philo_id;
	int				id;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*alive_monitor;
	pthread_mutex_t	*message;
	int				live;
	int				eat_count;
	int				n_eat;
	struct timeval	last_eat;
	struct timeval	sim_start;
	t_state			state;
}	t_philo;
typedef struct s_data
{
	t_philo	*head_philo;
	t_args	*sim_param;
}t_data;
int	ft_atoi(char *str);
int	simulation(t_args *sim_params);
#endif
