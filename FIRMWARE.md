Updating FIRMWARE
=================

Userspace program is located in zet6221_firmware_userspace.c

* Disable reset pin (set it low)
  ./pio -m 'PB5<1><0><1><0>'
* msleep(1)
* send password
  u8 ts_sndpwd_cmd[3] = {0x20,0xC5,0x9D};
  i2c_master_send(client, ts_sndpwd_cmd, 3)
* msleep(200)
* send sfr
  u8 ts_cmd[1] = {0x2C};
  u8 ts_in_data[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  u8 ts_cmd17[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  i2c_master_write(client, ts_cmd, 1);

  i2c_master_rec(client, ts_in_data, 16);
  msleep(100);

  for (i=0;i<16;i++)
  {
	ts_cmd17[i+1]=ts_in_data[i];
  }

  if (ts_in_data[14]==0x3D)
  {
	ts_cmd[0]=0x2D;
	ret=zet6221_i2c_write_tsdata(client, ts_cmd, 1);
	msleep(500);
	ts_cmd17[15]=0x7D;
	ts_cmd17[0]=0x2E;
	ret=zet6221_i2c_write_tsdata(client, ts_cmd17, 17);
  } else {
	ts_cmd17[15]=0x3D;
	ts_cmd17[0]=0x2B;
	ret=zet6221_i2c_write_tsdata(client, ts_cmd17, 17);
  }

* msleep(200)
* zet6221_ts_masserase
  
  u8 ts_cmd[1] = {0x24};
  i2c_master_write(client, ts_cmd, 1);
  
* msleep(200)
* Juggle the firmware in position

  BufLen=sizeof(zeitec_zet6221_firmware)/sizeof(char);
  while(BufLen>0)
  {
	memset(zeitec_zet6221_page,0x00,130);
	if(BufLen>128)
	{
		for(i=0;i<128;i++)
		{
			zeitec_zet6221_page[i+2]=zeitec_zet6221_firmware[BufIndex];
			BufIndex+=1;
		}
		zeitec_zet6221_page[0]=0x22;
		zeitec_zet6221_page[1]=BufPage;
		BufLen-=128;
	}
	else
	{
		for(i=0;i<BufLen;i++)
		{
			zeitec_zet6221_page[i+2]=zeitec_zet6221_firmware[BufIndex];
			BufIndex+=1;
		}
		zeitec_zet6221_page[0]=0x22;
		zeitec_zet6221_page[1]=BufPage;
		BufLen=0;
	}

	ret=zet6221_i2c_write_tsdata(client,
			zeitec_zet6221_page,
			130);
	msleep(200);
	BufPage+=1;
  }

  * enable irq again (GPIO Pin)
  ./pio -m 'PB5<4><0><1><1>'

  * msleep(200);

  * zet6221_ts_resetmcu

  u8 ts_cmd[1] = {0x29};
  ret=i2c_master_write(client, ts_cmd, 1);
