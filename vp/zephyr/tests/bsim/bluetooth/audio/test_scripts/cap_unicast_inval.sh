#!/usr/bin/env bash
#
# Copyright (c) 2023 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0

SIMULATION_ID="cap_unicast_inval"
VERBOSITY_LEVEL=2

source ${ZEPHYR_BASE}/tests/bsim/sh_common.source

cd ${BSIM_OUT_PATH}/bin

printf "\n\n======== Running CAP unicast test =========\n\n"

Execute ./bs_${BOARD_TS}_tests_bsim_bluetooth_audio_prj_conf \
  -v=${VERBOSITY_LEVEL} -s=${SIMULATION_ID} -d=0 -testid=cap_initiator_unicast_inval -rs=46 -D=2

Execute ./bs_${BOARD_TS}_tests_bsim_bluetooth_audio_prj_conf \
  -v=${VERBOSITY_LEVEL} -s=${SIMULATION_ID} -d=1 -testid=cap_acceptor_unicast -rs=23 -D=2

# Simulation time should be larger than the WAIT_TIME in common.h
Execute ./bs_2G4_phy_v1 -v=${VERBOSITY_LEVEL} -s=${SIMULATION_ID} \
  -D=2 -sim_length=60e6 $@

wait_for_background_jobs