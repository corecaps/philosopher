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
	int				n_philo;
	int				ttdie;
	int				tteat;
	int				ttsleep;
	int				max_eat;
	int				running;
	pthread_mutex_t	*message;
	pthread_mutex_t	*live_check;
}	t_args;
typedef enum e_state
{
	THINKING,
	EATING,
	SLEEPING,
	DEAD,
	FORK
}	t_state;
typedef struct s_philo
{
	pthread_t		philo_id;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*run_state_check;
	pthread_mutex_t	*message;
	int				id;
	int				*sim_running;
	int				n_eat;
	int				max_eat;
	int				ttdie;
	int				tteat;
	int				ttsleep;
	int				n_philo;
	struct timeval	last_eat;
	struct timeval	sim_start;
	t_state			state;
}	t_philo;
int			ft_atoi(char *str);
int			simulation(t_args *sim_params);
void		message(enum e_state action, long int stamp, int running,
				t_philo *philo);
long int	get_stamp(struct timeval start);
void		think(t_philo *philo);
void		eat(t_philo *philo);
void		sleeping(t_philo *philo);
void		clean(t_args *sim_params, t_philo *philo);
#endif