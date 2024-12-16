#include <stdio.h>
#include "soc.h"
#include "audiodac.h"
#include "utils.h"
#include "riscv.h"

short *audiodac0_tx_ring_buffer = NULL;
int audiodac0_tx_ring_buffer_size = 0;
int audiodac0_tx_ring_buffer_playback_ptr = 0;
int audiodac0_tx_ring_buffer_fill_ptr = 0;

int audiodac0_submit_buffer(short *audio_buffer, int audio_buffer_fill, int isr_flag) {

	int processed = 0;

	if(audiodac0_tx_ring_buffer == NULL || audio_buffer == NULL || audiodac0_tx_ring_buffer_size == 0) {
		printf("audiodac0_submit_buffer: fail, audiodac0_tx_ring_buffer = %p, audio_buffer = %p, audiodac0_tx_ring_buffer_size = %d\r\n", audiodac0_tx_ring_buffer_size, audio_buffer, audiodac0_tx_ring_buffer_size);
		return 0;
	}

	if(audiodac0_tx_ring_buffer_fill_ptr == audiodac0_tx_ring_buffer_playback_ptr - 1) {
		print("audiodac0_submit_buffer: tx ring buffer is full!\r\n");
		return 0;
	}

	if(audiodac0_tx_ring_buffer_fill_ptr == audiodac0_tx_ring_buffer_playback_ptr)
		print("audiodac0_submit_buffer: tx ring buffer is empty!\r\n");

	if(!isr_flag)
		csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	if(audiodac0_tx_ring_buffer_fill_ptr >= audiodac0_tx_ring_buffer_playback_ptr) {
		int low_part_size  = MIN(audio_buffer_fill, audiodac0_tx_ring_buffer_size - audiodac0_tx_ring_buffer_fill_ptr);
		int high_part_size = audio_buffer_fill - low_part_size; 

		//printf("audiodac0_submit_buffer: low_part_size = %d, high_part_size = %d\r\n", low_part_size, high_part_size);

		if(!isr_flag)
			csr_set(mstatus, MSTATUS_MIE); // Temporarily enable interrupts while copying 

		for(int i = 0; i < low_part_size; i++) 
			audiodac0_tx_ring_buffer[audiodac0_tx_ring_buffer_fill_ptr + i] = *audio_buffer++;

		if(!isr_flag)
			csr_clear(mstatus, MSTATUS_MIE); // Disable interrupts 

		audiodac0_tx_ring_buffer_fill_ptr += low_part_size;
		audiodac0_tx_ring_buffer_fill_ptr %= audiodac0_tx_ring_buffer_size;

		processed += low_part_size;

		if(high_part_size) {
			high_part_size = MIN(high_part_size, audiodac0_tx_ring_buffer_playback_ptr);

			if(!isr_flag)
				csr_set(mstatus, MSTATUS_MIE); // Temporarily enable interrupts while copying 

			for(int i = 0; i < high_part_size; i++) 
				audiodac0_tx_ring_buffer[i] = *audio_buffer++;

			if(!isr_flag)
				csr_clear(mstatus, MSTATUS_MIE); // Disable interrupts 

			audiodac0_tx_ring_buffer_fill_ptr += high_part_size;

			processed += high_part_size;
		}
	} else {
		int low_part_size = MIN(audio_buffer_fill, audiodac0_tx_ring_buffer_playback_ptr -
				audiodac0_tx_ring_buffer_fill_ptr - 1); 

		if(!isr_flag)
			csr_set(mstatus, MSTATUS_MIE); // Temporarily enable interrupts while copying 

		for(int i = 0; i < low_part_size; i++) 
			audiodac0_tx_ring_buffer[audiodac0_tx_ring_buffer_fill_ptr + i] = *audio_buffer++;
		
		if(!isr_flag)
			csr_clear(mstatus, MSTATUS_MIE); // Disable interrupts 

		audiodac0_tx_ring_buffer_fill_ptr += low_part_size;

		processed += low_part_size;
	}


	//if(audiodac0_tx_ring_buffer_fill_ptr == audiodac0_tx_ring_buffer_playback_ptr - 1)
	//	printf("audiodac0_submit_buffer: tx buffer is full!\r\n");

	if(AUDIODAC0->status & AUDIO_DAC_STATUS_CMD_INT) {
		print("audiodac0_submit_buffer: restarting FIFO\r\n");
		audiodac0_isr(); // Restart FIFO
	}

	if(!isr_flag)
		csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	//printf("audiodac0_submit_buffer: processed = %d, audiodac0_tx_ring_buffer_playback_ptr = %d, audiodac0_tx_ring_buffer_fill_ptr = %d\r\n", processed, audiodac0_tx_ring_buffer_playback_ptr, audiodac0_tx_ring_buffer_fill_ptr);

	return processed;
}

void audiodac0_start_playback(short *ring_buffer, int ring_buffer_size) {

	if(!ring_buffer || ring_buffer_size < 1)
		return;

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	AUDIODAC0->status &= ~(AUDIO_DAC_STATUS_CMD_INT_HALF_ENABLE | AUDIO_DAC_STATUS_CMD_INT_ENABLE); 

	audiodac0_tx_ring_buffer = ring_buffer;
	audiodac0_tx_ring_buffer_fill_ptr = 0; //ring_buffer_size - 1;
	audiodac0_tx_ring_buffer_playback_ptr = 0;
	audiodac0_tx_ring_buffer_size = ring_buffer_size;

	audiodac0_isr();

	AUDIODAC0->status |= AUDIO_DAC_STATUS_CMD_INT_HALF_ENABLE | AUDIO_DAC_STATUS_CMD_INT_ENABLE; 

	printf("audiodac0_start_playback: done\r\n");

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts
}

void audiodac0_stop_playback(void) {
	AUDIODAC0->status &= ~(AUDIO_DAC_STATUS_CMD_INT_HALF_ENABLE | AUDIO_DAC_STATUS_CMD_INT_ENABLE); 
}

int audiodac0_isr(void) {
	int samples_to_send = 0, samples_sent = 0;

	if(AUDIODAC0->status & AUDIO_DAC_STATUS_CMD_INT) {
		print("audiodac0_isr: tx FIFO is empty!\r\n");
	}

	if(audiodac0_tx_ring_buffer_fill_ptr == audiodac0_tx_ring_buffer_playback_ptr) {
		print("audiodac0_isr: tx buffer underrun!\r\n");
		goto end;
	}

	if((samples_to_send = audiodac_get_tx_avail(AUDIODAC0)/3) < 1) {
		print("audiodac0_isr: tx FIFO is full!\r\n");
		goto end;
	}


	if(audiodac0_tx_ring_buffer_fill_ptr > audiodac0_tx_ring_buffer_playback_ptr)
		samples_to_send = MIN(samples_to_send, audiodac0_tx_ring_buffer_fill_ptr -
				audiodac0_tx_ring_buffer_playback_ptr);
	else 
		// audiodac0_tx_ring_buffer_fill_ptr < audiodac0_tx_ring_buffer_playback_ptr
		samples_to_send = MIN(samples_to_send, audiodac0_tx_ring_buffer_size -
				audiodac0_tx_ring_buffer_playback_ptr);



	samples_sent = audiodac_xmit(AUDIODAC0, audiodac0_tx_ring_buffer + audiodac0_tx_ring_buffer_playback_ptr,
			samples_to_send, 1000);

		
	//printf("audiodac0_isr: playback_ptr = %d, fill_ptr = %d, samples_to_send = %d, samples_sent = %d\r\n",
	//		audiodac0_tx_ring_buffer_playback_ptr, audiodac0_tx_ring_buffer_fill_ptr,
	//		samples_to_send, samples_sent);
	

	audiodac0_tx_ring_buffer_playback_ptr += samples_sent;
	audiodac0_tx_ring_buffer_playback_ptr %= audiodac0_tx_ring_buffer_size;

	end:

	//if(samples_sent == 0)
	//	print("audiodac0_isr: samples_sent = 0\r\n");

	AUDIODAC0->status &= ~(AUDIO_DAC_STATUS_CMD_INT | AUDIO_DAC_STATUS_CMD_HALF_EMPTY_INT); 

	return samples_sent;
}

void audiodac_init(AUDIODAC_Reg* reg, int sample_rate) {
	AUDIODAC_Config audiodac_cfg;

	//audiodac_cfg.config = AUDIO_DAC_CONFIG_CPHA | AUDIO_DAC_CONFIG_CPOL;
	audiodac_cfg.config = AUDIO_DAC_CONFIG_CPHA;
	audiodac_cfg.ssSetup = 16;
	audiodac_cfg.ssHold = 16;
	audiodac_cfg.ssDisable = 16;
	//audiodac_cfg.divider = SYSTEM_CLOCK_HZ/(2*sample_rate*(16 + 1));
	audiodac_cfg.divider = SYSTEM_CLOCK_HZ/(2*sample_rate*(16 + 0));
	audiodac_applyConfig(reg, &audiodac_cfg);

	reg->data = AUDIO_DAC_CMD_ENABLE_SS0;
	reg->data = 0b1000000000000000; // Gain = OFF, Buf = OFF, Vdd ref = OFF
	reg->data = AUDIO_DAC_CMD_DISABLE_SS0;

	reg->data = AUDIO_DAC_CMD_ENABLE_SS0;
	reg->data = 0b1100000000000000; // Power Down = OFF
	reg->data = AUDIO_DAC_CMD_DISABLE_SS0;

	reg->data = AUDIO_DAC_CMD_ENABLE_SS0;
	reg->data = 0b1010000000000000; // nLDAC = Low - continuous update
	reg->data = AUDIO_DAC_CMD_DISABLE_SS0;

	printf("audiodac_init: divider = %d\r\n", audiodac_cfg.divider);
}

int audiodac_wait_tx_avail(AUDIODAC_Reg* reg, int min_avail, int timeout) {
	int i;
	int avail;

	for(i = 0; i < timeout; i++) {
		if((avail = audiodac_get_tx_avail(reg)) >= min_avail)
			break;
	}

	if(i == timeout)
		return 0;

	return avail;
}

int audiodac_xmit(AUDIODAC_Reg* reg, short* buf, int len, int timeout) {

	int avail, sent = 0;

	/*
	if((avail = audiodac_wait_tx_avail(reg, 3, timeout)) < 0)
		return 0;

	reg->data = AUDIO_DAC_CMD_ENABLE_SS0;
	reg->data = 0b1010000000000000; // nLDAC = Low - contimuous update
	reg->data = AUDIO_DAC_CMD_DISABLE_SS0;
	*/

	while(len > 0) {
		//if((avail = audiodac_wait_tx_avail(reg, 3, timeout)) < 3)
		if((avail = audiodac_get_tx_avail(reg)) < 3)
			break;

		for(int i = 0; i < MIN(avail/3, len); i++) {
			reg->data = AUDIO_DAC_CMD_ENABLE_SS0;
			reg->data = ((unsigned int)(*buf++ + 0x8000) >> 4) | AUDIODAC0_CHANNEL; // conver to 12 bit, add channel 
			reg->data = AUDIO_DAC_CMD_DISABLE_SS0;
			len--;
			sent++;
		}
	}

	return sent;
}



