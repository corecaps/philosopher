//
// Created by corecaps on 31/01/23.
//

#include "philo.h"

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