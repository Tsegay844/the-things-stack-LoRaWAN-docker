/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"
#include "subghz_phy_version.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  TB_RX,
  TB_RX_DONE,
  TB_RX_SYNCH,
  TB_TX,
  TB_TX_DONE,
  TB_TX_TIMEOUT,
  TB_TX_SYNCH,
  TB_WAIT_USER_TRIG,
  TB_END,
  TB_ERROR,
} Testbench_States_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TESTBENCH
#define TEST_MODE_CFG		0  // 0 for Tx Mode, else Rx Mode

/** DR_CHANGE_MANUAL is used to set the test automatic radio reconfiguration
 * 	In Tx Mode, when in Manual Mode,  the program stops at the end of each Power cycle.
 * 	The user then must use button2 (Push Button 2) to continue the test.
 * 	After pressing the button, the program then Configures the Radio with the next DR value
 * 	and starts a new Power Cycle.
 * 	In Rx Mode, when in Manual Mode, the program does not star the Synch Timer, used to change
 * 	the Radio DR config. In this mode, the Radio will remain DEFAULT_DATA_RATE, listening to
 * 	incoming packets.
 * */
#define DR_CHANGE_MANUAL	1	// 0 for Auto mode, else Manual Mode

#define MAX_LORA_DR				6  // the testbench will evaluate DR0 to DR6 from RP002-1.0.0 EU863-870 Data Rate
#define DEFAULT_DATA_RATE		5
#define MAX_TX_OUTPUT_POWER		16	/* dBm */
#define MIN_TX_OUTPUT_POWER		6	/* dBm */
#define DEFAULT_TX_OUTPUT_POWER		14	/* dBm */

#define TEST_TX_POWER_STEP			2	// dBm step when changing TX power
#define TEST_TX_PKT_INTERVAL_MS		200  // delay added after every transmission (in ms)
#define TEST_N_PKTS		3	// number of packets sent every cycle (uint16_t)
#define TB_PAYLOAD_LEN	64 	// bytes
// RX CYCLE TIME CONFIGURATION
#define RX_CYCLE_TIME_DR0	2500 // HARDCODED IN MS

/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              300 // Used For Synch State (0 for continuous)
#define TX_TIMEOUT_VALUE              10000

/*Size of the payload to be sent*/
/* Size must be greater of equal the PING and PONG*/
#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
//TESTBENCH
int tx_power_dbm = DEFAULT_TX_OUTPUT_POWER;
int lora_data_rate = DEFAULT_DATA_RATE;
int n_tx_ctr = 0; // in Tx Mode, counts pkts sent per Cycle. In Rx mode, counts pkts received per Cycle
static Testbench_States_t Testbench_State = TB_WAIT_USER_TRIG;
bool rx_cycle_complete_flag = true; // set in the Synch Timer callback
bool tx_synch_flag = true; // set in the Tb_OnRxDone
bool rx_synch_flag = true; // set in the Tb_OnRxDone
uint32_t button1_event = 0;
uint32_t button2_event = 0;
uint32_t tx_start_timestamp = 0; // RTC count
uint32_t cycle_start_timestamp = 0; // RTC count
uint32_t rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0; // initial Synch Timer value
static UTIL_TIMER_Object_t timerRxSynch; // Synch Timer for changing DR in Rx Mode

/* App Rx Buffer*/
static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
/* App Tx Buffer*/
static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
/* Last  Received Buffer Size*/
uint16_t RxBufferSize = 0;
/* Last  Received packer Rssi*/
int8_t RssiValue = 0;
/* Last  Received packer SNR (in Lora modulation)*/
int8_t SnrValue = 0;

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
//TESTBENCH
static void Tb_Tx_Process(void);
static void Tb_OnTxDone(void);
static void Tb_OnTxTimeout(void);

static void Tb_Rx_Process(void);
static void Tb_OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);
static void Tb_OnRxTimeout(void);
static void Tb_OnRxError(void);
static void Tb_OnTimerRxSynch(void *context);

void Tb_Config_Radio(int new_tx_power_dbm, int new_data_rate);

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
  APP_LOG(TS_OFF, VLEVEL_M, "\n---------------\rLORA RADIO TESTBENCH FOR SIGNAL ATTENUATION MEASUREMENT\r");
  if (TEST_MODE_CFG == 0){
	  APP_LOG(TS_OFF, VLEVEL_M, "TRANSMITTER MODE CONFIGURATION\n\r");
  }
  else{
	  APP_LOG(TS_OFF, VLEVEL_M, "RECEIVER MODE CONFIGURATION\n\r");
  }
  /* Get SubGHY_Phy APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  // Rx Synch Timer
  UTIL_TIMER_Create(&timerRxSynch, rx_synch_timer_value_ms, UTIL_TIMER_ONESHOT, Tb_OnTimerRxSynch, NULL);
  // Push Buttons
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);

  /* USER CODE END SubghzApp_Init_1 */
  /* Radio initialization */
  RadioEvents.TxDone = Tb_OnTxDone;
  RadioEvents.RxDone = Tb_OnRxDone;
  RadioEvents.TxTimeout = Tb_OnTxTimeout;
  RadioEvents.RxError = Tb_OnRxError;
  RadioEvents.RxTimeout = Tb_OnRxTimeout;

  Radio.Init(&RadioEvents);
  /* USER CODE BEGIN SubghzApp_Init_2 */
  /*calculate random delay for synchronization*/
  //random_delay = (Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);
  /* Radio configuration */
  Tb_Config_Radio(DEFAULT_TX_OUTPUT_POWER, DEFAULT_DATA_RATE);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);
  /*fills tx buffer*/
  memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  /*register task to to be run in while(1) after Radio IT*/
  if (TEST_MODE_CFG == 0){
	  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, Tb_Tx_Process);
  }
  else{
	  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, Tb_Rx_Process);
  }
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);

  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN PrFD */
void Tb_Config_Radio(int new_tx_power_dbm, int new_data_rate)
{
	int lora_spreading_factor = 12;
	int lora_bandwidth = 0; // 125 kHz

	if (new_tx_power_dbm < MIN_TX_OUTPUT_POWER || new_tx_power_dbm > MAX_TX_OUTPUT_POWER){
		APP_LOG(TS_OFF, VLEVEL_M, "LORA CONFIG: POWER CONFIGURATION OUT OF RANGE\n\r");
		new_tx_power_dbm = DEFAULT_TX_OUTPUT_POWER;
	}

	Radio.Sleep();
	// based on RP002-1.0.0 LoRaWAN Regional Parameters
	// EU863-870 Data Rate and End-device Output Power encoding
	switch(new_data_rate){
		case 0:
			// Radio default configuration is DR0, with SF12, BW 125 kHz, CR 4/6
			break;
		case 1:
			lora_spreading_factor = 11;
			lora_bandwidth = 0; // 125 kHz
			break;
		case 2:
			lora_spreading_factor = 10;
			lora_bandwidth = 0; // 125 kHz
			break;
		case 3:
			lora_spreading_factor = 9;
			lora_bandwidth = 0; // 125 kHz
			break;
		case 4:
			lora_spreading_factor = 8;
			lora_bandwidth = 0; // 125 kHz
			break;
		case 5:
			lora_spreading_factor = 7;
			lora_bandwidth = 0; // 125 kHz
			break;
		case 6:
			lora_spreading_factor = 7;
			lora_bandwidth = 1; // 250 kHz
			break;
		default:
			APP_LOG(TS_OFF, VLEVEL_M, "LORA CONFIG: DATA RATE OUT OF RANGE\n\r");
		    break;
	}

	/* Radio re configuration */
	#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
	  APP_LOG(TS_OFF, VLEVEL_M, "\n---------------\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "RADIO CONFIG\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "TX_OUTPUT_POWER=%d dBm\r", new_tx_power_dbm);
	  APP_LOG(TS_OFF, VLEVEL_M, "LORA DATA RATE=%d\r", new_data_rate);
	  APP_LOG(TS_OFF, VLEVEL_M, "LORA_BW=%d kHz\r", (1 << lora_bandwidth) * 125);
	  APP_LOG(TS_OFF, VLEVEL_M, "LORA_SF=%d\r", lora_spreading_factor);

	  Radio.SetTxConfig(MODEM_LORA, new_tx_power_dbm, 0, lora_bandwidth,
			            lora_spreading_factor, LORA_CODINGRATE,
	                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

	  Radio.SetRxConfig(MODEM_LORA, lora_bandwidth, lora_spreading_factor,
	                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
	                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
	                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

	#else
	#error "Testbench only set for LoRa configuration - subghz_phy_app.h file."
	#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
}


static void Tb_OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  uint32_t current_time = HAL_GetTick();
  float delta = (float)(current_time - tx_start_timestamp)/1024; // transmission time on air in seconds
  int deltaInt = (int)delta;
  int deltaDec = (int)((delta - deltaInt) * 1000);
  /* Update the State of the FSM*/
  if (Testbench_State == TB_TX){
	  Testbench_State = TB_TX_DONE;
	  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnTxDone - TOA=%ds%03d\r", deltaInt, deltaDec);
  }
  else if(Testbench_State == TB_TX_SYNCH){
	  // Start testbench DR cycle
	  Testbench_State = TB_TX_DONE;
  }
  else if(Testbench_State == TB_RX_SYNCH){
	  rx_synch_flag = true; // star synch receiver - wait for TX Node to send REPLY synch frame
  	  Testbench_State = TB_RX_DONE;
    }

  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}


static void Tb_OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnTxTimeout\n\r");
  /* Update the State of the FSM*/
  Testbench_State = TB_TX_TIMEOUT;

  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}


static void Tb_OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  //APP_LOG(TS_ON, VLEVEL_L, "Tb_OnRxDone\n\r");
  /* Clear BufferRx*/
  memset(BufferRx, 0, MAX_APP_BUFFER_SIZE);
  /* Record payload size*/
  RxBufferSize = size;
  if (RxBufferSize <= MAX_APP_BUFFER_SIZE)
  {
    memcpy(BufferRx, payload, RxBufferSize);
  }
  /* Record Received Signal Strength*/
  // RssiValue = rssi;
  /* Record payload content*/
  APP_LOG(TS_ON, VLEVEL_L, "RSSI=%ddBm; SNR=%ddB; PAYLOAD(%d): KEY(%x%x%x) CTR(%02x%02x) DR(%02x) TXPWR(%02x)\n\r",
		  rssi, LoraSnr_FskCfo, size,
		  BufferRx[0], BufferRx[1], BufferRx[2], BufferRx[4], BufferRx[3], BufferRx[5], BufferRx[6]);

  /* Update the State of the FSM*/
    if (Testbench_State == TB_RX){ // Regular reception during Testbench run
  	Testbench_State = TB_RX_DONE;
    }
    else if(Testbench_State == TB_TX_SYNCH){ // TX synch sate - Waiting for RX Node to send synch frame
  	  // check payload key value
  	  if (BufferRx[0] == 0xaf){
  		tx_synch_flag = true; // Received Cycle synch frame
  	  }
    }
    else if(Testbench_State == TB_RX_SYNCH){ // RX synch sate - Waiting for TX Node to send REPLY synch frame
    	// check payload key value
    	if (BufferRx[0] == 0xbf){
    		tx_synch_flag = true; // Received REPLY synch frame
    	}
    	if (DR_CHANGE_MANUAL == 0){
    		// RX Synch timer
    		UTIL_TIMER_Start(&timerRxSynch);
    		APP_LOG(TS_ON, VLEVEL_L, "TEST START - AUTO CFG\n\r");
    	}
    }

  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxDone */
}


static void Tb_OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnRxError: PKT ERROR\n\r");
  /* Update the State of the FSM*/
  Testbench_State = TB_ERROR;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}


static void Tb_OnRxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnRxTimeout\n\r");

  if(Testbench_State == TB_RX_SYNCH){
	  rx_synch_flag = false; // FAILED TO GET REPLY synch frame from TX Node
	  // send new synch frame
  }

  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}


static void Tb_Tx_Process(void){
	uint32_t cycle_end_timestamp;
	float delta;
	int deltaInt;
	int deltaDec;

	switch (Testbench_State){
		case TB_WAIT_USER_TRIG:
			Radio.Sleep();
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led
			while(button1_event == 0){
				__NOP();
			};
			button1_event = 0;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // BLUE Led

			tx_synch_flag = false;
			Testbench_State = TB_TX_SYNCH;
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_TX_SYNCH:
			if(tx_synch_flag == false){ // Wait for RX node to send initial PKT
				Tb_Config_Radio(DEFAULT_TX_OUTPUT_POWER, DEFAULT_DATA_RATE); // Set radio to default parameters
				HAL_Delay(50);
				Radio.Rx(0);
			}
			else{ // Received Cycle start frame
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // GREEN Led
				HAL_Delay(200);
				memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE); // clear buffer
				BufferTx[0] = 0xbf; // key for synch frame
				Radio.Send(BufferTx, TB_PAYLOAD_LEN); // send REPLY synch frame - callback set FSM to TB_RX
			}
			break;

		case TB_TX:
			HAL_Delay(TEST_TX_PKT_INTERVAL_MS);
			//HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // BLUE Led
			memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE); // clear buffer
			BufferTx[0] = 0xc0;
			BufferTx[1] = 0xff;
			BufferTx[2] = 0xee;
			BufferTx[3] = n_tx_ctr & 0xff;
			BufferTx[4] = (n_tx_ctr >> 8) & 0xff;
			BufferTx[5] = lora_data_rate & 0xff;
			BufferTx[6] = tx_power_dbm & 0xff;

			if (n_tx_ctr == 0){
				cycle_start_timestamp = HAL_GetTick();
				APP_LOG(TS_ON, VLEVEL_L, "Cycle START - PWR=%d dBm - DR %d\r", tx_power_dbm, lora_data_rate);
			}
			APP_LOG(TS_ON, VLEVEL_L, "TX SEND - PAYLOAD: KEY(%x%x%x) CTR(%02x%02x) DR(%02x) TXPWR(%02x) LEN(%d)\r",
					BufferTx[0], BufferTx[1], BufferTx[2], BufferTx[4], BufferTx[3], BufferTx[5], BufferTx[6], TB_PAYLOAD_LEN);
			tx_start_timestamp = HAL_GetTick();
			Radio.Send(BufferTx, TB_PAYLOAD_LEN);
			break;

		case TB_TX_TIMEOUT: // the test should always use TX_DONE callback
			Testbench_State = TB_ERROR; // Error - TX timeout should not be called
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_TX_DONE:
			n_tx_ctr++; // increment pkt counter
			Radio.Sleep();
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led

			if (tx_synch_flag == true){ // done transmitting REPLY synch frame
				tx_synch_flag = false; // reset synch flag - once the cycle ends TX Node needs to be re-synch for the next DR
				n_tx_ctr = 0;
				// Configure the Radio for the Next DR Cycle - Start from default PWR
				Tb_Config_Radio(DEFAULT_TX_OUTPUT_POWER, lora_data_rate);
				Testbench_State = TB_TX;
			}
			else if (n_tx_ctr < TEST_N_PKTS){ // continue TX
				// HAL_Delay(TEST_TX_PKT_INTERVAL_MS);
				Testbench_State = TB_TX;
			}
			else if (n_tx_ctr == TEST_N_PKTS){ // end of PWR cycle
				cycle_end_timestamp = HAL_GetTick();
				delta = (float)(cycle_end_timestamp - cycle_start_timestamp)/1024;
				deltaInt = (int)delta;
				deltaDec = (int)((delta - deltaInt) * 1000);
				APP_LOG(TS_ON, VLEVEL_L, "Cycle END - PWR=%d dBm - DR %d\r - Total time: %ds%03d",
						tx_power_dbm, lora_data_rate, deltaInt, deltaDec);
				n_tx_ctr = 0; // reset pkt counter
				Testbench_State = TB_TX; // return to TX state
				if (tx_power_dbm > MIN_TX_OUTPUT_POWER){
					tx_power_dbm = tx_power_dbm - TEST_TX_POWER_STEP;
					Tb_Config_Radio(tx_power_dbm, lora_data_rate); // decrease TX OUTPUT POWER, same DR
				}
				else if (tx_power_dbm == MIN_TX_OUTPUT_POWER && lora_data_rate < MAX_LORA_DR){ // completed all PWR cycles
					tx_power_dbm = DEFAULT_TX_OUTPUT_POWER;
					lora_data_rate++; // increase DR for next Cycle
					HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
					if(DR_CHANGE_MANUAL == 0){ // Auto config
						tx_synch_flag = false;
						Testbench_State = TB_TX_SYNCH; // Return To synch State
						//HAL_Delay(2000);
					}
					else{ // Manual config - Next DR is set after button2 is pressed
						Tb_Config_Radio(DEFAULT_TX_OUTPUT_POWER, lora_data_rate);
						Testbench_State = TB_TX;
						button2_event = 0;
						while(button2_event == 0){ // wait for button2 to be pressed
							__NOP();
						};
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // GREEN Led
					}
				}
				else if (tx_power_dbm == MIN_TX_OUTPUT_POWER && lora_data_rate == MAX_LORA_DR){ // completed all PWR OUTPUT AND DR CHANGES
					Testbench_State = TB_END;
				}
				else {
					Testbench_State = TB_ERROR; // Error
				}
			}
			else {
				Testbench_State = TB_ERROR; // Error
			}
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_END:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); // RED Led
			break;

		case TB_ERROR:
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); // RED Led
			break;

		default:
			break;
	}

}


static void Tb_Rx_Process(void){
	switch (Testbench_State){
		case TB_WAIT_USER_TRIG:
			Radio.Sleep();
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); // RED Led
			while(button1_event == 0){
				__NOP();
			};
			button1_event = 0;
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); // RED Led

			rx_synch_flag = false;
			Testbench_State = TB_RX_SYNCH;
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_RX_SYNCH:
			if (rx_synch_flag == false){
				memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE); // clear buffer
				BufferTx[0] = 0xaf; // key for synch frame
				Radio.Send(BufferTx, TB_PAYLOAD_LEN); // send REPLY synch frame - callback set FSM to TB_RX
				HAL_Delay(50);
			}
			else{
				Radio.Rx(RX_TIMEOUT_VALUE);
			}

			break;

		case TB_RX:
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // GREEN Led
			if (n_tx_ctr == 0){
				cycle_start_timestamp = HAL_GetTick();
				rx_cycle_complete_flag = false;
				APP_LOG(TS_ON, VLEVEL_L, "Cycle START - DR %d\n\r", lora_data_rate);
			}
			// APP_LOG(TS_ON, VLEVEL_L, "TESTBENCH RADIO RX SET\n\r");
			Radio.Rx(0); // Continuous Mode
			break;

		case TB_RX_DONE:
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
			Radio.Sleep();
			HAL_Delay(50);
			n_tx_ctr++; // increment received packet count - reset is done in the synch timer callback
			Testbench_State = TB_RX; // resume RX
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_END:
			APP_LOG(TS_ON, VLEVEL_L, "TEST END - OK\n\r");
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); // RED Led
			break;

		case TB_ERROR:
			APP_LOG(TS_ON, VLEVEL_L, "TEST ERROR\n\r");
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); // RED Led
			break;

		default:
			break;

	}

}


static void Tb_OnTimerRxSynch(void *context){
	uint32_t cycle_end_timestamp;
	cycle_end_timestamp = HAL_GetTick();
	float delta = (cycle_end_timestamp - cycle_start_timestamp)/1024;
	int deltaInt = (int)delta;
	int deltaDec = (int)((delta - deltaInt) * 1000);
	// This function is called at the end of a Full TX Power Cycle - Move to Next LoRa Data Rate
	APP_LOG(TS_ON, VLEVEL_L, "Cycle END - Packets Received (%d) - Cycle Time %ds%03d\n\r",
			n_tx_ctr, deltaInt, deltaDec);
	n_tx_ctr = 0; // reset counter
	if (lora_data_rate < MAX_LORA_DR){
		lora_data_rate++;
		Tb_Config_Radio(tx_power_dbm, lora_data_rate); // Configure Radio with the next DR value
		// Configure Synch Timer for the next DR value
		// TODO MOVE THIS TO Tb_OnRxDone - Timer must be set from the Synch State
		switch(lora_data_rate){
					case 0:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 1:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 2:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 3:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 4:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 5:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					case 6:
						rx_synch_timer_value_ms = RX_CYCLE_TIME_DR0;
						break;
					default:
						APP_LOG(TS_OFF, VLEVEL_M, "RX SYNCH: DATA RATE OUT OF RANGE\n\r");
					    break;
				}
			UTIL_TIMER_SetPeriod(&timerRxSynch, rx_synch_timer_value_ms);
			UTIL_TIMER_Start(&timerRxSynch);

			Testbench_State = TB_RX; // resume RX
	}
	else if (lora_data_rate == MAX_LORA_DR){
		Testbench_State = TB_END;
	}

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_SW1_PIN)
  {
    button1_event = 1;
  }
  else if (GPIO_Pin == BUTTON_SW2_PIN)
  {
	  button2_event = 1;
  }
}
/* USER CODE END PrFD */
