#!/usr/bin/python

import sys
import sqlite3

db = sqlite3.connect(sys.argv[1])
run_numbers = sys.argv[2:]

#Numbers from Andy's calibration
silicon = {
	'SiL1-X-S_gains' : [1.91548, 2.31305, 2.48751, 2.46837, 2.43467, 2.41007, 2.44776, 2.39563, 2.444, 2.44166, 2.47957, 2.51718, 2.52069, 2.40918, 2.44792, 1.99945],
	'SiL1-X-S_offsets' : [1001.92, 79.653, 78.4198, 68.3483, 60.8002, 54.9057, 64.0799, 44.9943, 59.7204, 48.1753, 59.9305, 58.8384, 70.6312, 51.4737, 52.7079, 999.998],
	'SiL3-S_gains' : [4.36673],
	'SiL3-S_offsets' : [35.17],
	'SiR1-X-S_gains' : [1.77056,1.75883, 1.79166, 1.76022],
	'SiR1-X-S_offsets' : [-23.1492, -9.25994, -36.4741, -50.4722],
	'SiR2-S_gains' : [4.2416],
	'SiR2-S_offsets' : [107.348],
	'SiR3-S_gains' : [5.75035],
	'SiR3-S_offsets' : [58.5218],
	'SiT-X-S_gains' : [2.02316, 1.91805, 1.94223, 1.95524],
	'SiT-X-S_offsets' : [-8.01687, 12.4836, -11.7933, 0.85092]
}

#Numbers from Eu125
germanium = {
	'GeLoGain_gains':{
		9302:0.396515,
		10319:0.399114
	},
        'GeLoGain_gainerrs':{
		9302:0.00000129689,
		10319:0.0000111315
	},
	'GeLoGain_offsets':{
		9302:0.261953,
		10319:0.229208
	},
	'GeLoGain_offseterrs':{
		9302:0.020722,
		10319:0.0155383
	},
	'GeHiGain_gains':{
		9302:0.184892,
		10319:0.185084
	},
        'GeHiGain_gainerrs':{
		9302:0.00000543024,
		10319:0.00000477178
	},
	'GeHiGain_offsets':{
		9302:-0.896645,
		10319:0.114321
	},
	'GeHiGain_offseterrs':{
		9302:0.0407438,
		10319:0.0144359
	}
}
def assign_ge_calib(run_number):
	#Decide with Germanium numbers to use based on run number
	if int(run_number) < 10319:
		ge_calib = 9302
	else:
		ge_calib = 10319
	return ge_calib
	
def execute_sqls_for(channel, table_name='Energy'):
	arrGains = silicon[channel + "_gains"];
	arrOffsets = silicon[channel + "_offsets"];
	#For each run and channel, check if entry exists to UPDATE or INSERT if it does not exist
	for i in range(0, len(arrGains) ):
		channel_id = channel.replace('X', str(i+1) )
		cmd = "SELECT * FROM " + table_name + " WHERE run==? AND channel=?"
		args = (run_number, channel_id)
		cur = db.execute(cmd, args)
		if cur.fetchone():
			cmd = "UPDATE " + table_name + " SET gain=?, offset=? WHERE run==? AND channel==?" 
			args = (arrGains[i], arrOffsets[i], run_number, channel_id)
		else:
			cmd = "INSERT INTO " + table_name + "(run, channel, gain, offset) VALUES (?, ?, ?, ?)"
			args = (run_number, channel_id, arrGains[i], arrOffsets[i])
		with db:
			db.execute(cmd, args)

def execute_sqls_with_err(channel, table_name='Energy'):
	ge_calib = assign_ge_calib(run_number)
	arrGain = germanium[channel + "_gains"][ge_calib];
	arrOffset = germanium[channel + "_offsets"][ge_calib];
	arrGainErr = germanium[channel + "_gainerrs"][ge_calib];
	arrOffsetErr = germanium[channel + "_offseterrs"][ge_calib];
	#For each run and channel, check if entry exists to UPDATE or INSERT if it does not exist
	cmd = "SELECT * FROM " + table_name + " WHERE run==? AND channel=?"
	args = (run_number, channel)
	cur = db.execute(cmd, args)
	if cur.fetchone():
		cmd = "UPDATE " + table_name + " SET gain=?, gain_err=?, offset=?, offset_err=? WHERE run==? AND channel==?" 
		args = (arrGain, arrGainErr, arrOffset, arrOffsetErr, run_number, channel)
	else:
		cmd = "INSERT INTO " + table_name + "(run, channel, gain, gain_err, offset, offset_err) VALUES (?, ?, ?, ?, ?, ?)"
		args = (run_number, channel, arrGain, arrGainErr, arrOffset, arrOffsetErr)
	with db:
		db.execute(cmd, args)

if len(sys.argv) < 2:
    print "Usage: ./insert_energy_calibration.py DATABASE runNumber1 runNumber2 runNumber3..."
    exit(0)

#Fill or update energy calibration data for various detectors
for run_number in run_numbers:
	table_name = 'Energy'
	db.execute("CREATE TABLE IF NOT EXISTS " + table_name + "(run INT, channel TEXT, 'gain' REAL, 'gain_err' REAL, 'offset' REAL, 'offset_err' REAL, 'chi2' REAL, 'ndf' REAL);")
	execute_sqls_for('SiL1-X-S', 'Energy')
	execute_sqls_for('SiL3-S', 'Energy')
	execute_sqls_for('SiR1-X-S', 'Energy')
	execute_sqls_for('SiR2-S', 'Energy')
	execute_sqls_for('SiR3-S', 'Energy')
	execute_sqls_for('SiT-X-S', 'Energy')
	execute_sqls_with_err('GeLoGain', 'Energy')
	execute_sqls_with_err('GeHiGain', 'Energy')
