//
// Created by corecaps on 31/01/23.
//

#include "philo.h"

void message(enum e_state action, long int stamp, int id)
{
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
	message(THINKING, get_stamp(philo->sim_start),philo->id);
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
	if (philo->live == 0)
	{
		pthread_mutex_unlock(philo->alive_monitor);
		return;
	}
	pthread_mutex_unlock(philo->alive_monitor);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->alive_monitor);
		if (philo->live > 0)
			message(FORK, get_stamp(philo->sim_start),philo->id);
		pthread_mutex_unlock(philo->alive_monitor);
		pthread_mutex_lock(left_philo_ptr->right_fork);
		message(FORK,get_stamp(philo->sim_start),philo->id);
	}
	else
	{
		pthread_mutex_lock(left_philo_ptr->right_fork);
		pthread_mutex_lock(philo->alive_monitor);
		if (philo->live > 0)
			message(FORK,get_stamp(philo->sim_start),philo->id);
		pthread_mutex_unlock(philo->alive_monitor);
		pthread_mutex_lock(philo->right_fork);
		message(FORK,get_stamp(philo->sim_start),philo->id);
	}

	pthread_mutex_lock(philo->alive_monitor);
	if (philo->live > 0)
		message(EATING,get_stamp(philo->sim_start),philo->id);
	gettimeofday(&now,NULL);
	philo->last_eat = now;
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
	message(SLEEPING,get_stamp(philo->sim_start),philo->id);
	usleep(philo->sim_params->ttsleep * 1000);
	philo->state = THINKING;
}

void	*philosopher(void *arg)
{
	t_philo		*philo;


	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->alive_monitor);
	while (philo->live)
	{
//		printf("live :%d\n",philo->live);
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
	return (NULL);
}

void	*alive_monitor(void *arg)
{
	int				i;
	int				running;
	t_data			*data;
	long int		stamp;
	int				j;

	data = (t_data *) arg;
	running = 1;
	while (running)
	{
		i = 0;
		while (i < data->sim_param->n_philo)
		{
			pthread_mutex_lock((data->head_philo+i)->alive_monitor);
			stamp = get_stamp((data->head_philo+i)->last_eat);
//			printf("[Philo_%d] last_meal %ld\t ttdie %d\n",i+1,stamp,data->sim_param->ttdie);
			if (stamp > data->sim_param->ttdie)
			{
				(data->head_philo+i)->live = 0;
				pthread_mutex_unlock((data->head_philo+i)->alive_monitor);
				stamp = get_stamp((data->head_philo+i)->sim_start);
				message(DEAD,stamp,i+1);
				j = 0;
				while (j < data->sim_param->n_philo)
				{
					pthread_mutex_lock((data->head_philo + j)->alive_monitor);
					(data->head_philo + j)->live = 0;
//					printf("philo off:%d\n",j + 1);
					pthread_mutex_unlock((data->head_philo + j)->alive_monitor);
					j ++;
				}
				running = 0;
				break;
			}
			else
				pthread_mutex_unlock((data->head_philo+i)->alive_monitor);
			i ++;
		}
		usleep((data->sim_param->ttdie / 2));
	}
	return (NULL);
}

int	simulation(t_args *sim_params)
{
	t_philo		*philo;
	int			i;
	struct timeval	start;
	t_data		*data;
	pthread_t	alive_mon;

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
		philo[i].n_eat = 0;
		philo[i].live = 1;
		philo[i].id = i + 1;
		philo[i].sim_params = sim_params;
		philo[i].head = philo;
		philo[i].sim_start = start;
		gettimeofday(&philo[i].last_eat, NULL);
		philo[i].state = THINKING;
		philo[i].right_fork = malloc(sizeof(pthread_mutex_t));
		philo[i].alive_monitor = malloc(sizeof(pthread_mutex_t));
		if (!philo[i].right_fork || !philo[i].alive_monitor)
			return (-1);
		pthread_mutex_init(philo[i].right_fork, NULL);
		pthread_mutex_init(philo[i].alive_monitor, NULL);
		i++;
	}
	i = 0;
	while (i ++ < sim_params->n_philo)
		pthread_create(&philo[i-1].philo_id, NULL, philosopher, &philo[i-1]);
	pthread_create(&alive_mon,NULL,alive_monitor,data);
	sleep(10);
	return (0);
}