#ifndef hamming_H
#define hamming_H


// Window Coefficients 

float Wind[] = {
	0.080000000 ,0.080139632 ,0.080558444 ,0.081256180 ,0.082232418 ,
	0.083486566 ,0.085017860 ,0.086825373 ,0.088908006 ,0.091264495 ,
	0.093893409 ,0.096793154 ,0.099961967 ,0.103397926 ,0.107098944 ,
	0.111062774 ,0.115287011 ,0.119769089 ,0.124506288 ,0.129495732 ,
	0.134734391 ,0.140219085 ,0.145946484 ,0.151913112 ,0.158115346 ,
	0.164549420 ,0.171211429 ,0.178097329 ,0.185202937 ,0.192523942 ,
	0.200055898 ,0.207794233 ,0.215734248 ,0.223871124 ,0.232199921 ,
	0.240715582 ,0.249412937 ,0.258286707 ,0.267331503 ,0.276541836 ,
	0.285912112 ,0.295436645 ,0.305109651 ,0.314925258 ,0.324877507 ,
	0.334960356 ,0.345167684 ,0.355493294 ,0.365930917 ,0.376474217 ,
	0.387116792 ,0.397852183 ,0.408673870 ,0.419575286 ,0.430549810 ,
	0.441590782 ,0.452691497 ,0.463845217 ,0.475045170 ,0.486284557 ,
	0.497556555 ,0.508854319 ,0.520170992 ,0.531499704 ,0.542833575 ,
	0.554165727 ,0.565489278 ,0.576797356 ,0.588083093 ,0.599339640 ,
	0.610560161 ,0.621737846 ,0.632865908 ,0.643937592 ,0.654946175 ,
	0.665884975 ,0.676747351 ,0.687526708 ,0.698216503 ,0.708810244 ,
	0.719301502 ,0.729683906 ,0.739951154 ,0.750097012 ,0.760115322 ,
	0.770000000 ,0.779745046 ,0.789344544 ,0.798792666 ,0.808083676 ,
	0.817211933 ,0.826171896 ,0.834958125 ,0.843565286 ,0.851988154 ,
	0.860221615 ,0.868260671 ,0.876100441 ,0.883736166 ,0.891163210 ,
	0.898377064 ,0.905373349 ,0.912147817 ,0.918696356 ,0.925014990 ,
	0.931099882 ,0.936947340 ,0.942553812 ,0.947915896 ,0.953030335 ,
	0.957894025 ,0.962504013 ,0.966857501 ,0.970951846 ,0.974784561 ,
	0.978353320 ,0.981655957 ,0.984690466 ,0.987455005 ,0.989947896 ,
	0.992167626 ,0.994112846 ,0.995782376 ,0.997175203 ,0.998290480 ,
	0.999127531 ,0.999685848 ,0.999965091 ,0.999965091 ,0.999685848 ,
	0.999127531 ,0.998290480 ,0.997175203 ,0.995782376 ,0.994112846 ,
	0.992167626 ,0.989947896 ,0.987455005 ,0.984690466 ,0.981655957 ,
	0.978353320 ,0.974784561 ,0.970951846 ,0.966857501 ,0.962504013 ,
	0.957894025 ,0.953030335 ,0.947915896 ,0.942553812 ,0.936947340 ,
	0.931099882 ,0.925014990 ,0.918696356 ,0.912147817 ,0.905373349 ,
	0.898377064 ,0.891163210 ,0.883736166 ,0.876100441 ,0.868260671 ,
	0.860221615 ,0.851988154 ,0.843565286 ,0.834958125 ,0.826171896 ,
	0.817211933 ,0.808083676 ,0.798792666 ,0.789344544 ,0.779745046 ,
	0.770000000 ,0.760115322 ,0.750097012 ,0.739951154 ,0.729683906 ,
	0.719301502 ,0.708810244 ,0.698216503 ,0.687526708 ,0.676747351 ,
	0.665884975 ,0.654946175 ,0.643937592 ,0.632865908 ,0.621737846 ,
	0.610560161 ,0.599339640 ,0.588083093 ,0.576797356 ,0.565489278 ,
	0.554165727 ,0.542833575 ,0.531499704 ,0.520170992 ,0.508854319 ,
	0.497556555 ,0.486284557 ,0.475045170 ,0.463845217 ,0.452691497 ,
	0.441590782 ,0.430549810 ,0.419575286 ,0.408673870 ,0.397852183 ,
	0.387116792 ,0.376474217 ,0.365930917 ,0.355493294 ,0.345167684 ,
	0.334960356 ,0.324877507 ,0.314925258 ,0.305109651 ,0.295436645 ,
	0.285912112 ,0.276541836 ,0.267331503 ,0.258286707 ,0.249412937 ,
	0.240715582 ,0.232199921 ,0.223871124 ,0.215734248 ,0.207794233 ,
	0.200055898 ,0.192523942 ,0.185202937 ,0.178097329 ,0.171211429 ,
	0.164549420 ,0.158115346 ,0.151913112 ,0.145946484 ,0.140219085 ,
	0.134734391 ,0.129495732 ,0.124506288 ,0.119769089 ,0.115287011 ,
	0.111062774 ,0.107098944 ,0.103397926 ,0.099961967 ,0.096793154 ,
	0.093893409 ,0.091264495 ,0.088908006 ,0.086825373 ,0.085017860 ,
	0.083486566 ,0.082232418 ,0.081256180 ,0.080558444 ,0.080139632 ,
	0.080000000 };
float Wind_16[] = {
	0.080000000 ,0.119769089 ,0.232199921 ,0.397852183 ,0.588083093 ,
	0.770000000 ,0.912147817 ,0.989947896 ,0.989947896 ,0.912147817 ,
	0.770000000 ,0.588083093 ,0.397852183 ,0.232199921 ,0.119769089 ,
	0.080000000 };
#endif