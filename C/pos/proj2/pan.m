#define rand	pan_rand
#if defined(HAS_CODE) && defined(VERBOSE)
	printf("Pr: %d Tr: %d\n", II, t->forw);
#endif
	switch (t->forw) {
	default: Uerror("bad forward move");
	case 0:	/* if without executable clauses */
		continue;
	case 1: /* generic 'goto' or 'skip' */
		IfNotBlocked
		_m = 3; goto P999;
	case 2: /* generic 'else' */
		IfNotBlocked
		if (trpt->o_pm&1) continue;
		_m = 3; goto P999;

		 /* PROC P */
	case 3: /* STATE 1 - line 18 "pan_in" - [count[_pid] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][1] = 1;
		(trpt+1)->bup.oval = now.count[ Index(((int)((P0 *)this)->_pid), 3) ];
		now.count[ Index(((P0 *)this)->_pid, 3) ] = 0;
#ifdef VAR_RANGES
		logval("count[_pid]", now.count[ Index(((int)((P0 *)this)->_pid), 3) ]);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 4: /* STATE 2 - line 23 "pan_in" - [flag[_pid] = 2] (0:0:1 - 5) */
		IfNotBlocked
		reached[0][2] = 1;
		(trpt+1)->bup.oval = now.flag[ Index(((int)((P0 *)this)->_pid), 3) ];
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = 2;
#ifdef VAR_RANGES
		logval("flag[_pid]", now.flag[ Index(((int)((P0 *)this)->_pid), 3) ]);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 5: /* STATE 3 - line 25 "pan_in" - [((turn!=_pid))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][3] = 1;
		if (!((now.turn!=((int)((P0 *)this)->_pid))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 6: /* STATE 4 - line 27 "pan_in" - [((flag[turn]==0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][4] = 1;
		if (!((now.flag[ Index(now.turn, 3) ]==0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 7: /* STATE 5 - line 27 "pan_in" - [turn = _pid] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][5] = 1;
		(trpt+1)->bup.oval = now.turn;
		now.turn = ((int)((P0 *)this)->_pid);
#ifdef VAR_RANGES
		logval("turn", now.turn);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 8: /* STATE 15 - line 34 "pan_in" - [flag[_pid] = 1] (0:0:1 - 3) */
		IfNotBlocked
		reached[0][15] = 1;
		(trpt+1)->bup.oval = now.flag[ Index(((int)((P0 *)this)->_pid), 3) ];
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = 1;
#ifdef VAR_RANGES
		logval("flag[_pid]", now.flag[ Index(((int)((P0 *)this)->_pid), 3) ]);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 9: /* STATE 16 - line 35 "pan_in" - [j = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][16] = 1;
		(trpt+1)->bup.oval = ((P0 *)this)->j;
		((P0 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P:j", ((P0 *)this)->j);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 10: /* STATE 17 - line 37 "pan_in" - [((j<3))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][17] = 1;
		if (!((((P0 *)this)->j<3)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 11: /* STATE 18 - line 39 "pan_in" - [(((j!=_pid)&&(flag[j]==1)))] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][18] = 1;
		if (!(((((P0 *)this)->j!=((int)((P0 *)this)->_pid))&&(now.flag[ Index(((P0 *)this)->j, 3) ]==1))))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P0 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P0 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 12: /* STATE 21 - line 40 "pan_in" - [j = (j+1)] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][21] = 1;
		(trpt+1)->bup.oval = ((P0 *)this)->j;
		((P0 *)this)->j = (((P0 *)this)->j+1);
#ifdef VAR_RANGES
		logval("P:j", ((P0 *)this)->j);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 13: /* STATE 29 - line 47 "pan_in" - [KS = (KS+1)] (0:0:1 - 3) */
		IfNotBlocked
		reached[0][29] = 1;
		(trpt+1)->bup.oval = now.KS;
		now.KS = (now.KS+1);
#ifdef VAR_RANGES
		logval("KS", now.KS);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 14: /* STATE 30 - line 49 "pan_in" - [count[_pid] = (count[_pid]+1)] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][30] = 1;
		(trpt+1)->bup.oval = now.count[ Index(((int)((P0 *)this)->_pid), 3) ];
		now.count[ Index(((P0 *)this)->_pid, 3) ] = (now.count[ Index(((int)((P0 *)this)->_pid), 3) ]+1);
#ifdef VAR_RANGES
		logval("count[_pid]", now.count[ Index(((int)((P0 *)this)->_pid), 3) ]);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 15: /* STATE 31 - line 50 "pan_in" - [assert((KS==1))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][31] = 1;
		assert((now.KS==1), "(KS==1)", II, tt, t);
		_m = 3; goto P999; /* 0 */
	case 16: /* STATE 32 - line 51 "pan_in" - [KS = (KS-1)] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][32] = 1;
		(trpt+1)->bup.oval = now.KS;
		now.KS = (now.KS-1);
#ifdef VAR_RANGES
		logval("KS", now.KS);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 17: /* STATE 33 - line 54 "pan_in" - [flag[_pid] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][33] = 1;
		(trpt+1)->bup.oval = now.flag[ Index(((int)((P0 *)this)->_pid), 3) ];
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = 0;
#ifdef VAR_RANGES
		logval("flag[_pid]", now.flag[ Index(((int)((P0 *)this)->_pid), 3) ]);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 18: /* STATE 34 - line 57 "pan_in" - [((count[_pid]<4))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][34] = 1;
		if (!((now.count[ Index(((int)((P0 *)this)->_pid), 3) ]<4)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 19: /* STATE 36 - line 58 "pan_in" - [((count[_pid]>=4))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][36] = 1;
		if (!((now.count[ Index(((int)((P0 *)this)->_pid), 3) ]>=4)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 20: /* STATE 40 - line 60 "pan_in" - [-end-] (0:0:0 - 2) */
		IfNotBlocked
		reached[0][40] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */
	case  _T5:	/* np_ */
		if (!((!(trpt->o_pm&4) && !(trpt->tau&128))))
			continue;
		/* else fall through */
	case  _T2:	/* true */
		_m = 3; goto P999;
#undef rand
	}

