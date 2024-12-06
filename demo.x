// Actually, it wouldn't be that complex. It's easier. 
//			S.			V.			O.								Adv.
			:			import		std						,		as (:)			. // "." as the sentence is over. 
			:			eval		( (var named (a)) = 0 )	.
			:			print		(var named (a))			,		fortimes (5)		.
			:			print		(var named (i))			,		for (i from 0 to 4 step 0.05)	. // Special optimization for numbers. 
			:			print		(var named (i))			,		for (i in (var withval ([1,2,3,4,5,6,7,8,9,10]))		.
