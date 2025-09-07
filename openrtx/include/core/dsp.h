/***************************************************************************
 *   Copyright (C) 2020 - 2025 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN,                     *
 *                                Silvano Seva IU2KWO,                     *
 *                                Frederik Saraci IU2NRO                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef DSP_H
#define DSP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int16_t audio_sample_t;

/*
 * This header contains various DSP utilities which can be used to condition
 * input or output signals when implementing digital modes on OpenRTX.
 */

/**
 * Data structure holding the internal state of a filter.
 */
typedef struct
{
    float u[3];         // input values u(k), u(k-1), u(k-2)
    float y[3];         // output values y(k), y(k-1), y(k-2)
    bool  initialised;  // state variables initialised
}
filter_state_t;

/**
 * Reset the filter state variables.
 *
 * @param state: pointer to the data structure containing the filter state.
 */
void dsp_resetFilterState(filter_state_t *state);

/**
 * Remove the DC offset from a collection of audio samples, processing data
 * in-place.
 *
 * @param state: pointer to the data structure containing the filter state.
 * @param buffer: buffer containing the audio samples.
 * @param length: number of samples contained in the buffer.
 */
void dsp_dcRemoval(filter_state_t *state, audio_sample_t *buffer, size_t length);

/*
 * Inverts the phase of the audio buffer passed as paramenter.
 * The buffer will be processed in place to save memory.
 *
 * @param buffer: the buffer to be used as both source and destination.
 * @param length: the length of the input buffer.
 */
void dsp_invertPhase(audio_sample_t *buffer, uint16_t length);

/**
 * Data structure holding the internal state of a signal power squelch gate.
 */
struct pwrSquelch {
    uint32_t filtOut;
    bool gate;
};

/**
 * Initialize the internal state of a signal power squelch gate.
 *
 * @param sq: pointer to state data structure.
 */
void dsp_pwrSquelchInit(struct pwrSquelch *sq);

/**
 * Update the internal state of a signal power squelch gate.
 * The outut of the envelope filter is the normalized signal power, ranging
 * from 0 to 65535.
 *
 * @param sq: pointer to state data structure.
 * @param sample: new data sample.
 * @param alpha: time constant for the envelope lowpass filter.
 * @return filter output after update.
 */
uint16_t dsp_pwrSquelchUpdate(struct pwrSquelch *sq, int16_t sample,
                              uint16_t alpha);

/**
 * Evaluate the gate conditions of a signal power squelch gate.
 * The squelch gate opens if the current envelope is greater than openThr and
 * closes if lower than closeThr.
 *
 * @param sq: pointer to state data structure.
 * @param openThr: gate opening threshold.
 * @param closeThr: gate closing threshold.
 * @return gate state.
 */
bool dsp_pwrSquelchEvaluate(struct pwrSquelch *sq, uint16_t openThr,
                            uint16_t closeThr);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* DSP_H */
