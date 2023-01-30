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

static void	usage(void)
{
	printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat");
	printf(" time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
	printf("\ttimes are in milliseconds\n");
	printf("\tnumber_of_philosophers must be greater than 1\n");
	exit(1);
}

int	main(int argc, char **argv)
{
	(void)argv;
	if (argc != 5 && argc != 6)
		usage();
	return (0);
}