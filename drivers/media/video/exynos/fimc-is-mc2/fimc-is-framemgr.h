/*
 * Samsung Exynos5 SoC series FIMC-IS driver
 *
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FIMC_IS_FRAME_MGR_H
#define FIMC_IS_FRAME_MGR_H

#define FRAMEMGR_ID_SENSOR	0x100 /*256*/
#define FRAMEMGR_ID_ISP		0x200 /*512*/
#define FRAMEMGR_ID_SCC		0x400
#define FRAMEMGR_ID_SCP		0x800

/*#define TRACE_FRAME*/
/*#define TRACE_ID		(0xF00)*/
#define TRACE_ID		(0x0)

#define FRAMEMGR_MAX_REQUEST	20

/*flite frame start tasklet*/
#define FMGR_IDX_0		(0x10)
/*flite frame end tasklet*/
#define FMGR_IDX_1		(0x20)
/*sensor queue*/
#define FMGR_IDX_2		(0x30)
/*sensor dequeue*/
#define FMGR_IDX_3		(0x40)
/*dev framedone*/
#define FMGR_IDX_4		(0x50)
/*scc framedone*/
#define FMGR_IDX_5		(0x60)
/*scp framedone*/
#define FMGR_IDX_6		(0x70)
/*isp framedone*/
#define FMGR_IDX_7		(0x80)
/*scc callback*/
#define FMGR_IDX_8		(0x90)
/*scp callback*/
#define FMGR_IDX_9		(0xA0)

#define framemgr_e_barrier_irqs(this, index, flag) \
	spin_lock_irqsave(&this->slock, flag)
#define framemgr_x_barrier_irqr(this, index, flag) \
	spin_unlock_irqrestore(&this->slock, flag)
#define framemgr_e_barrier_irq(this, index) \
	spin_lock_irq(&this->slock)
#define framemgr_x_barrier_irq(this, index) \
	spin_unlock_irq(&this->slock)
#define framemgr_e_barrier(this, index) \
	spin_lock(&this->slock)
#define framemgr_x_barrier(this, index) \
	spin_unlock(&this->slock)

enum fimc_is_frame_output {
	FIMC_IS_FOUT_NONE,
	FIMC_IS_FOUT_REQ,
	FIMC_IS_FOUT_DONE,
};

enum fimc_is_frame_shot_state {
	FIMC_IS_FRAME_STATE_FREE,
	FIMC_IS_FRAME_STATE_REQUEST,
	FIMC_IS_FRAME_STATE_PROCESS,
	FIMC_IS_FRAME_STATE_COMPLETE,
	FIMC_IS_FRAME_STATE_INVALID
};

enum fimc_is_frame_reqeust {
	/* SCC, SCP frame done,
	   ISP meta done */
	REQ_FRAME,
	/* ISP shot done */
	REQ_SHOT
};

enum fimc_is_frame_init {
	/* uninitialized memory */
	FRAME_UNI_MEM,
	/* initialized memory */
	FRAME_INI_MEM,
	/* configured memory */
	FRAME_CFG_MEM
};

struct fimc_is_frame_shot {
	struct list_head list;

	/* sensor and isp use */
	enum fimc_is_frame_init init;
	struct camera2_shot *shot;
	struct camera2_shot_ext *shot_ext;
	u32 kvaddr_shot;
	u32 dvaddr_shot;
	u32 cookie_shot;
	u32 shot_size;

	/* stream use */
	struct camera2_stream *stream;
	u32 stream_size;

	/* common use */
	u32 planes;
	u32 kvaddr_buffer[4];
	u32 dvaddr_buffer[4];

	/* time measure */
	struct timeval *tzone;

	/* internal use */
	u32 state;
	u32 fcount;
	u32 index;
	unsigned long req_flag;
	struct vb2_buffer *vb;

	enum fimc_is_frame_output scc_out;
	enum fimc_is_frame_output scp_out;

	/* time measure internally */
	struct timeval			time_queued;
	struct timeval			time_shot;
	struct timeval			time_shotdone;
	struct timeval			time_dequeued;
};

struct fimc_is_framemgr {
	struct fimc_is_frame_shot	frame[FRAMEMGR_MAX_REQUEST];

	struct list_head		frame_free_head;
	struct list_head		frame_request_head;
	struct list_head		frame_process_head;
	struct list_head		frame_complete_head;

	spinlock_t			slock;

	u32				frame_cnt;
	u32				frame_free_cnt;
	u32				frame_request_cnt;
	u32				frame_process_cnt;
	u32				frame_complete_cnt;

	unsigned long			output_image_flag;
	u32				output_image_cnt;

	u32				id;
};

int fimc_is_frame_probe(struct fimc_is_framemgr *this, u32 id);
int fimc_is_frame_open(struct fimc_is_framemgr *this, u32 buffers);
int fimc_is_frame_close(struct fimc_is_framemgr *this);
void fimc_is_frame_print_all(struct fimc_is_framemgr *this);

int fimc_is_frame_s_free_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_g_free_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_free_head(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_print_free_list(struct fimc_is_framemgr *this);

int fimc_is_frame_s_request_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_g_request_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_request_head(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_print_request_list(struct fimc_is_framemgr *this);

int fimc_is_frame_s_process_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_g_process_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_process_head(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_print_process_list(struct fimc_is_framemgr *this);

int fimc_is_frame_s_complete_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_g_complete_shot(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_complete_head(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot **frame);
void fimc_is_frame_print_complete_list(struct fimc_is_framemgr *this);

int fimc_is_frame_trans_fre_to_req(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_fre_to_com(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_req_to_pro(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_req_to_com(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_req_to_fre(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *item);
int fimc_is_frame_trans_pro_to_com(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_pro_to_fre(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);
int fimc_is_frame_trans_com_to_fre(struct fimc_is_framemgr *this,
	struct fimc_is_frame_shot *frame);

#endif
