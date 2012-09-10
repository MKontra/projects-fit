	switch (t->back) {
	default: Uerror("bad return move");
	case  0: goto R999; /* nothing to undo */

		 /* PROC P */

	case 3: /* STATE 1 */
		;
		now.count[ Index(((P0 *)this)->_pid, 3) ] = trpt->bup.oval;
		;
		goto R999;

	case 4: /* STATE 2 */
		;
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		;
		;
		
	case 7: /* STATE 5 */
		;
		now.turn = trpt->bup.oval;
		;
		goto R999;

	case 8: /* STATE 15 */
		;
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = trpt->bup.oval;
		;
		goto R999;

	case 9: /* STATE 16 */
		;
		((P0 *)this)->j = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 11: /* STATE 18 */
		;
	/* 0 */	((P0 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;

	case 12: /* STATE 21 */
		;
		((P0 *)this)->j = trpt->bup.oval;
		;
		goto R999;

	case 13: /* STATE 29 */
		;
		now.KS = trpt->bup.oval;
		;
		goto R999;

	case 14: /* STATE 30 */
		;
		now.count[ Index(((P0 *)this)->_pid, 3) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 16: /* STATE 32 */
		;
		now.KS = trpt->bup.oval;
		;
		goto R999;

	case 17: /* STATE 33 */
		;
		now.flag[ Index(((P0 *)this)->_pid, 3) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		;
		;
		
	case 20: /* STATE 40 */
		;
		p_restor(II);
		;
		;
		goto R999;
	}

