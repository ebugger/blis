/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2013, The University of Texas

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

extern scalm_t*   scalm_cntl;

gemm_t*           gemm_cntl;

gemm_t*           gemm_cntl_bp_ke;
gemm_t*           gemm_cntl_op_bp;
gemm_t*           gemm_cntl_mm_op;
gemm_t*           gemm_cntl_vl_mm;

packm_t*          gemm_packa_cntl;
packm_t*          gemm_packb_cntl;
packm_t*          gemm_packc_cntl;
unpackm_t*        gemm_unpackc_cntl;

blksz_t*          gemm_mc;
blksz_t*          gemm_nc;
blksz_t*          gemm_kc;
blksz_t*          gemm_mr;
blksz_t*          gemm_nr;
blksz_t*          gemm_kr;
blksz_t*          gemm_extmr;
blksz_t*          gemm_extnr;
blksz_t*          gemm_extkr;
blksz_t*          gemm_ni;


void bli_gemm_cntl_init()
{
	// Create blocksize objects for each dimension.
	gemm_mc = bli_blksz_obj_create( BLIS_DEFAULT_MC_S,
	                                BLIS_DEFAULT_MC_D,
	                                BLIS_DEFAULT_MC_C,
	                                BLIS_DEFAULT_MC_Z );

	gemm_nc = bli_blksz_obj_create( BLIS_DEFAULT_NC_S,
	                                BLIS_DEFAULT_NC_D,
	                                BLIS_DEFAULT_NC_C,
	                                BLIS_DEFAULT_NC_Z );

	gemm_kc = bli_blksz_obj_create( BLIS_DEFAULT_KC_S,
	                                BLIS_DEFAULT_KC_D,
	                                BLIS_DEFAULT_KC_C,
	                                BLIS_DEFAULT_KC_Z );

	gemm_mr = bli_blksz_obj_create( BLIS_DEFAULT_MR_S,
	                                BLIS_DEFAULT_MR_D,
	                                BLIS_DEFAULT_MR_C,
	                                BLIS_DEFAULT_MR_Z );

	gemm_nr = bli_blksz_obj_create( BLIS_DEFAULT_NR_S,
	                                BLIS_DEFAULT_NR_D,
	                                BLIS_DEFAULT_NR_C,
	                                BLIS_DEFAULT_NR_Z );

	gemm_kr = bli_blksz_obj_create( BLIS_DEFAULT_KR_S,
	                                BLIS_DEFAULT_KR_D,
	                                BLIS_DEFAULT_KR_C,
	                                BLIS_DEFAULT_KR_Z );

	gemm_extmr = bli_blksz_obj_create( BLIS_EXTEND_MR_S,
	                                   BLIS_EXTEND_MR_D,
	                                   BLIS_EXTEND_MR_C,
	                                   BLIS_EXTEND_MR_Z );

	gemm_extnr = bli_blksz_obj_create( BLIS_EXTEND_NR_S,
	                                   BLIS_EXTEND_NR_D,
	                                   BLIS_EXTEND_NR_C,
	                                   BLIS_EXTEND_NR_Z );

	gemm_extkr = bli_blksz_obj_create( BLIS_EXTEND_KR_S,
	                                   BLIS_EXTEND_KR_D,
	                                   BLIS_EXTEND_KR_C,
	                                   BLIS_EXTEND_KR_Z );

	gemm_ni = bli_blksz_obj_create( BLIS_DEFAULT_NI_S,
	                                BLIS_DEFAULT_NI_D,
	                                BLIS_DEFAULT_NI_C,
	                                BLIS_DEFAULT_NI_Z );


	// Create control tree objects for packm operations on a, b, and c.
	gemm_packa_cntl
	=
	bli_packm_cntl_obj_create( BLIS_BLOCKED,
	                           BLIS_VARIANT2,
	                           gemm_mr, gemm_extmr,
	                           gemm_kr, gemm_extkr,
	                           FALSE, // do NOT scale by alpha
	                           FALSE, // already dense; densify not necessary
	                           FALSE, // do NOT invert diagonal
	                           FALSE, // reverse iteration if upper?
	                           FALSE, // reverse iteration if lower?
	                           BLIS_PACKED_ROW_PANELS,
	                           BLIS_BUFFER_FOR_A_BLOCK );

	gemm_packb_cntl
	=
	bli_packm_cntl_obj_create( BLIS_BLOCKED,
	                           BLIS_VARIANT2,
	                           gemm_kr, gemm_extkr,
	                           gemm_nr, gemm_extnr,
	                           FALSE, // do NOT scale by alpha
	                           FALSE, // already dense; densify not necessary
	                           FALSE, // do NOT invert diagonal
	                           FALSE, // reverse iteration if upper?
	                           FALSE, // reverse iteration if lower?
	                           BLIS_PACKED_COL_PANELS,
	                           BLIS_BUFFER_FOR_B_PANEL );

	gemm_packc_cntl
	=
	bli_packm_cntl_obj_create( BLIS_UNBLOCKED,
	                           BLIS_VARIANT1,
	                           gemm_mr, gemm_extmr,
	                           gemm_nr, gemm_extnr,
	                           FALSE, // do NOT scale by beta
	                           FALSE, // already dense; densify not necessary
	                           FALSE, // do NOT invert diagonal
	                           FALSE, // reverse iteration if upper?
	                           FALSE, // reverse iteration if lower?
	                           BLIS_PACKED_COLUMNS,
	                           BLIS_BUFFER_FOR_GEN_USE );

	gemm_unpackc_cntl
	=
	bli_unpackm_cntl_obj_create( BLIS_UNBLOCKED,
	                             BLIS_VARIANT1,
	                             NULL ); // no blocksize needed


	// Create control tree object for lowest-level block-panel kernel.
	gemm_cntl_bp_ke
	=
	bli_gemm_cntl_obj_create( BLIS_UNB_OPT,
	                          BLIS_VARIANT2,
	                          NULL, NULL, NULL, NULL,
	                          NULL, NULL, NULL, NULL );

	// Create control tree object for outer panel (to block-panel)
	// problem, packing a and b only.
	gemm_cntl_op_bp
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          //BLIS_VARIANT4,  // var1 with incremental pack in iter 0
	                          BLIS_VARIANT1,
	                          gemm_mc,
	                          gemm_ni,
	                          NULL,
	                          gemm_packa_cntl,
	                          gemm_packb_cntl,
	                          NULL,
	                          gemm_cntl_bp_ke,
	                          NULL );

	// Create control tree object for general problem via multiple
	// rank-k (outer panel) updates, packing a and b only.
	gemm_cntl_mm_op
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          BLIS_VARIANT3,
	                          gemm_kc,
	                          NULL,
	                          NULL,
	                          NULL, 
	                          NULL,
	                          NULL,
	                          gemm_cntl_op_bp,
	                          NULL );

	// Create control tree object for very large problem via multiple
	// general problems, packing a and b only.
	gemm_cntl_vl_mm
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          BLIS_VARIANT2,
	                          gemm_nc,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          gemm_cntl_mm_op,
	                          NULL );

	// Alias the "master" gemm control tree to a shorter name.
	//gemm_cntl = gemm_cntl_mm_op;
	gemm_cntl = gemm_cntl_vl_mm;
}

void bli_gemm_cntl_finalize()
{
	bli_blksz_obj_free( gemm_mc );
	bli_blksz_obj_free( gemm_nc );
	bli_blksz_obj_free( gemm_kc );
	bli_blksz_obj_free( gemm_mr );
	bli_blksz_obj_free( gemm_nr );
	bli_blksz_obj_free( gemm_kr );
	bli_blksz_obj_free( gemm_ni );

	bli_cntl_obj_free( gemm_packa_cntl );
	bli_cntl_obj_free( gemm_packb_cntl );
	bli_cntl_obj_free( gemm_packc_cntl );
	bli_cntl_obj_free( gemm_unpackc_cntl );

	bli_cntl_obj_free( gemm_cntl_bp_ke );
	bli_cntl_obj_free( gemm_cntl_op_bp );
	bli_cntl_obj_free( gemm_cntl_mm_op );
	bli_cntl_obj_free( gemm_cntl_vl_mm );
}

gemm_t* bli_gemm_cntl_obj_create( impl_t     impl_type,
                                  varnum_t   var_num,
                                  blksz_t*   b,
                                  blksz_t*   b_aux,
                                  scalm_t*   sub_scalm,
                                  packm_t*   sub_packm_a,
                                  packm_t*   sub_packm_b,
                                  packm_t*   sub_packm_c,
                                  gemm_t*    sub_gemm,
                                  unpackm_t* sub_unpackm_c )
{
	gemm_t* cntl;

	cntl = ( gemm_t* ) bli_malloc( sizeof(gemm_t) );	

	cntl->impl_type     = impl_type;
	cntl->var_num       = var_num;
	cntl->b             = b;
	cntl->b_aux         = b_aux;
	cntl->sub_scalm     = sub_scalm;
	cntl->sub_packm_a   = sub_packm_a;
	cntl->sub_packm_b   = sub_packm_b;
	cntl->sub_packm_c   = sub_packm_c;
	cntl->sub_gemm      = sub_gemm;
	cntl->sub_unpackm_c = sub_unpackm_c;

	return cntl;
}

