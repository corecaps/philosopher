/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgarcia <jgarcia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/30 09:48:22 by jgarcia           #+#    #+#             */
/*   Updated: 2023/01/30 09:48:35 by jgarcia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	usage(void)
{
	printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat");
	printf(" time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
	printf("\ttimes are in milliseconds\n");
	printf("\tnumber_of_philosophers must be greater than 1\n");
	return(1);
}

static int init_args(int argc, char **argv, t_args *args)
{
	args->n_philo = ft_atoi(argv[1]);
	args->ttdie = ft_atoi(argv[2]);
	args->tteat = ft_atoi(argv[3]);
	args->ttsleep = ft_atoi(argv[4]);
	if (args->n_philo < 2 || args->ttdie < 0 || args->tteat < 0
		|| args->ttsleep < 0)
		return (usage());
	if (argc == 6)
		args->n_eat = atoi(argv[5]);
	else
		args->n_eat = INT_MAX;
	args->alive = 1;
	return 0;
}

int	main(int argc, char **argv)
{
	t_args	sim_param;

	if (argc != 5 && argc != 6)
		return (usage());
	if (init_args(argc, argv, &sim_param))
		return (1);
	if (simulation(&sim_param) == -1)
		return (1);
	return (0);
}