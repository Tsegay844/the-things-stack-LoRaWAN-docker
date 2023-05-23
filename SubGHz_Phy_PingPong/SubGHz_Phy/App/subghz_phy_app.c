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
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;

typedef enum
{
  TB_RX_INIT,
  TB_RX,
  TB_RX_ERROR,
  TB_TX,
  TB_TX_DONE,
  TB_TX_TIMEOUT,
  TB_WAIT_USER_TRIG,
  TB_END,
  TB_ERROR,
} Testbench_States_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TESTBENCH
#define TEST_MODE_CFG	0  // 0 for Tx Mode, else Rx mode
#define MAX_LORA_DR		6  // the testbench will evaluate DR0 to DR6 from RP002-1.0.0 EU863-870 Data Rate
#define DR_CHANGE_MANUAL		0	// 0 for automatic DR change during test, else user must push button2 (Push Button 2)
#define MAX_TX_OUTPUT_POWER		16	/* dBm */
#define MIN_TX_OUTPUT_POWER		6	/* dBm */
#define TB_LOW_TX_OUTPUT_POWER		8   /* dBm */
#define TB_MID_TX_OUTPUT_POWER		10  /* dBm */
#define DEFAULT_TX_OUTPUT_POWER		14	/* dBm */
#define TEST_TX_PKT_INTERVAL_MS		200  // delay added after every transmission
#define TEST_N_PKTS		5	// number of packets sent every cycle (uint16_t)
#define TB_PAYLOAD_LEN	64 // bytes
// CALCULATING RX CYCLE TIME
#define DR0_TOA		1000
#define RX_CYCLE_TIME_DR0	(DR0_TOA + TEST_TX_PKT_INTERVAL_MS)*TEST_N_PKTS

/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              10000
#define TX_TIMEOUT_VALUE              10000
/* PING string*/
#define PING "PING"
/* PONG string*/
#define PONG "PONG"
/*Size of the payload to be sent*/
/* Size must be greater of equal the PING and PONG*/
#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */
/* wait for remote to be in Rx, before sending a Tx frame*/
#define RX_TIME_MARGIN                200
/* Afc bandwidth in Hz */
#define FSK_AFC_BANDWIDTH             83333
/* LED blink Period*/
#define LED_PERIOD_MS                 200

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
//TESTBENCH
int tx_power_dbm = DEFAULT_TX_OUTPUT_POWER; // default 14 dBm
int lora_data_rate = 0; // default DR0
int n_tx_ctr = 0;
static Testbench_States_t Testbench_State = TB_WAIT_USER_TRIG;
bool tx_complete_flag = true;
uint32_t button1_event = 0;
uint32_t button2_event = 0;
uint32_t tx_start_timestamp;
uint32_t cycle_start_timestamp;
static UTIL_TIMER_Object_t timerRxSynch; // synch timer for changing DR

/*Ping Pong FSM states */
static States_t State = RX;
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
/* Led Timers objects*/
static UTIL_TIMER_Object_t timerLed;
/* device state. Master: true, Slave: false*/
bool isMaster = true;
/* random delay to make sure 2 devices will sync*/
/* the closest the random delays are, the longer it will
   take for the devices to sync when started simultaneously*/
static int32_t random_delay;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
//static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
//static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief  Function executed on when led timer elapses
  * @param  context ptr of LED context
  */
static void OnledEvent(void *context);

/**
  * @brief PingPong state machine implementation
  */
static void PingPong_Process(void);

//TESTBENCH
static void Tb_OnTxDone(void);
static void Tb_OnTxTimeout(void);
static void Tb_Tx_Process(void);
static void Tb_Rx_Process(void);
static void Tb_OnTimerRxSynch(void *context);
void Tb_SubghzApp_ReConfig_Radio(int new_tx_power_dbm, int new_data_rate);

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  APP_LOG(TS_OFF, VLEVEL_M, "\n\rLORA RADIO TESTBENCH FOR SIGNAL ATTENUATION MEASUREMENT\r");
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

  /* Led Timers*/
  UTIL_TIMER_Create(&timerLed, LED_PERIOD_MS, UTIL_TIMER_ONESHOT, OnledEvent, NULL);
  UTIL_TIMER_Start(&timerLed);
  //TESTBENCH
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);

  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  //RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxDone = Tb_OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  //RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.TxTimeout = Tb_OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /*calculate random delay for synchronization*/
  //random_delay = (Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  /* Radio configuration */
#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
  APP_LOG(TS_OFF, VLEVEL_M, "\n---------------\r");
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\r");
  APP_LOG(TS_OFF, VLEVEL_M, "TX_OUTPUT_POWER=%d dBm\r", DEFAULT_TX_OUTPUT_POWER);
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_BW=%d kHz\r", (1 << LORA_BANDWIDTH) * 125);
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_SF=%d\r", LORA_SPREADING_FACTOR);

  Radio.SetTxConfig(MODEM_LORA, DEFAULT_TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

#elif ((USE_MODEM_LORA == 0) && (USE_MODEM_FSK == 1))
  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_BW=%d Hz\n\r", FSK_BANDWIDTH);
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_DR=%d bits/s\n\r", FSK_DATARATE);

  Radio.SetTxConfig(MODEM_FSK, DEFAULT_TX_OUTPUT_POWER, FSK_FDEV, 0,
                    FSK_DATARATE, 0,
                    FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, 0, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                    0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                    0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                    0, 0, false, true);

  Radio.SetMaxPayloadLength(MODEM_FSK, MAX_APP_BUFFER_SIZE);

#else
#error "Please define a modulation in the subghz_phy_app.h file."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

  /*fills tx buffer*/
  memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  //APP_LOG(TS_ON, VLEVEL_L, "rand=%d\n\r", random_delay);
  /*starts reception*/
  //Radio.Rx(RX_TIMEOUT_VALUE + random_delay);
  Radio.Sleep();
  /*register task to to be run in while(1) after Radio IT*/
  //UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, PingPong_Process);

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
//static void OnTxDone(void)
//{
//  /* USER CODE BEGIN OnTxDone */
//  APP_LOG(TS_ON, VLEVEL_L, "OnTxDone\n\r");
  /* Update the State of the FSM*/
//  State = TX;
  /* Run PingPong process in background*/
 // UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
//}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxDone\n\r");
#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
  APP_LOG(TS_ON, VLEVEL_L, "RssiValue=%d dBm, SnrValue=%ddB\n\r", rssi, LoraSnr_FskCfo);
  /* Record payload Signal to noise ratio in Lora*/
  SnrValue = LoraSnr_FskCfo;
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
#if ((USE_MODEM_LORA == 0) && (USE_MODEM_FSK == 1))
  APP_LOG(TS_ON, VLEVEL_L, "RssiValue=%d dBm, Cfo=%dkHz\n\r", rssi, LoraSnr_FskCfo);
  SnrValue = 0; /*not applicable in GFSK*/
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
  /* Update the State of the FSM*/
  State = RX;
  /* Clear BufferRx*/
  memset(BufferRx, 0, MAX_APP_BUFFER_SIZE);
  /* Record payload size*/
  RxBufferSize = size;
  if (RxBufferSize <= MAX_APP_BUFFER_SIZE)
  {
    memcpy(BufferRx, payload, RxBufferSize);
  }
  /* Record Received Signal Strength*/
  RssiValue = rssi;
  /* Record payload content*/
  APP_LOG(TS_ON, VLEVEL_H, "payload. size=%d \n\r", size);
  for (int32_t i = 0; i < PAYLOAD_LEN; i++)
  {
    APP_LOG(TS_OFF, VLEVEL_H, "%02X", BufferRx[i]);
    if (i % 16 == 15)
    {
      APP_LOG(TS_OFF, VLEVEL_H, "\n\r");
    }
  }
  APP_LOG(TS_OFF, VLEVEL_H, "\n\r");
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxDone */
}

//static void OnTxTimeout(void)
//{
  /* USER CODE BEGIN OnTxTimeout */
//  APP_LOG(TS_ON, VLEVEL_L, "OnTxTimeout\n\r");
  /* Update the State of the FSM*/
//  State = TX_TIMEOUT;
  /* Run PingPong process in background*/
//  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
//}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxTimeout\n\r");
  /* Update the State of the FSM*/
  State = RX_TIMEOUT;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxError\n\r");
  /* Update the State of the FSM*/
  State = RX_ERROR;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
void Tb_SubghzApp_ReConfig_Radio(int new_tx_power_dbm, int new_data_rate)
{
	int lora_spreading_factor = 12;
	int lora_bandwidth = 0; // 125 kHz

	if (new_tx_power_dbm < MIN_TX_OUTPUT_POWER || new_tx_power_dbm > MAX_TX_OUTPUT_POWER){
		APP_LOG(TS_OFF, VLEVEL_M, "LORA RECONFIG: POWER CONFIGURATION OUT OF RANGE\n\r");
		new_tx_power_dbm = 14;
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
			APP_LOG(TS_OFF, VLEVEL_M, "LORA RECONFIG: DATA RATE OUT OF RANGE\n\r");
		    break;
	}

	/* Radio re configuration */
	#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
	  APP_LOG(TS_OFF, VLEVEL_M, "\n---------------\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "RADIO RE-CONFIG\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\r");
	  APP_LOG(TS_OFF, VLEVEL_M, "TX_OUTPUT_POWER=%d dBm\r", new_tx_power_dbm);
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

static void Tb_OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  uint32_t current_time = HAL_GetTick();
  uint32_t tx_TOA = (current_time - tx_start_timestamp); // transmission time on air in milliseconds
  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnTxDone - TOA=%d ms\r", tx_TOA);
  /* Update the State of the FSM*/
  Testbench_State = TB_TX_DONE;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void Tb_OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "Tb_OnTxTimeout\n\r");
  /* Update the State of the FSM*/
  Testbench_State = TB_TX_TIMEOUT;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}


static void Tb_Tx_Process(void){
	uint32_t cycle_end_timestamp;
	switch (Testbench_State){
		case TB_WAIT_USER_TRIG:
			Radio.Sleep();
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led
			while(button1_event == 0){
				__NOP();
			};
			button1_event = 0;
			Testbench_State = TB_TX;
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // GREEN Led
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // BLUE Led
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			break;

		case TB_TX:
			HAL_Delay(TEST_TX_PKT_INTERVAL_MS);
			//HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // BLUE Led
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
			Radio.Sleep();
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // BLUE Led
			// HAL_Delay(TEST_TX_PKT_INTERVAL_MS);
			if (n_tx_ctr <= TEST_N_PKTS){
				Testbench_State = TB_TX;
				UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
			}
			break;

		case TB_TX_DONE:
			n_tx_ctr++; // increment pkt counter
			Radio.Sleep();
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // BLUE Led
			if (n_tx_ctr < TEST_N_PKTS){ // continue TX
				// HAL_Delay(TEST_TX_PKT_INTERVAL_MS);
				Testbench_State = TB_TX;
			}
			else if (n_tx_ctr == TEST_N_PKTS){ // end of cycle
				cycle_end_timestamp = HAL_GetTick();
				APP_LOG(TS_ON, VLEVEL_L, "Cycle END - PWR=%d dBm - DR %d\r - Total time: %d ms",
						tx_power_dbm, lora_data_rate, cycle_end_timestamp - cycle_start_timestamp);
				n_tx_ctr = 0; // reset pkt counter
				Testbench_State = TB_TX; // return to TX state
				if (tx_power_dbm == DEFAULT_TX_OUTPUT_POWER){
					tx_power_dbm = TB_MID_TX_OUTPUT_POWER;
					Tb_SubghzApp_ReConfig_Radio(tx_power_dbm, lora_data_rate); // decrease TX OUTPUT POWER, same DR
				}
				else if (tx_power_dbm == TB_MID_TX_OUTPUT_POWER){
					tx_power_dbm = TB_LOW_TX_OUTPUT_POWER;
					Tb_SubghzApp_ReConfig_Radio(tx_power_dbm, lora_data_rate); // decrease TX OUTPUT POWER, same DR
				}
				else if (tx_power_dbm == TB_LOW_TX_OUTPUT_POWER && lora_data_rate < MAX_LORA_DR){
					tx_power_dbm = DEFAULT_TX_OUTPUT_POWER;
					lora_data_rate++;
					Tb_SubghzApp_ReConfig_Radio(tx_power_dbm, lora_data_rate); // completed all PWR OUTPUT CHANGES - Increase DR
					if(DR_CHANGE_MANUAL == 0){
						HAL_Delay(2000); // delay to enable Rx to reconfigure
					}
					else{
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // GREEN Led
						button2_event = 0;
						while(button2_event == 0){
							__NOP();
						};
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // GREEN Led
					}
				}
				else if (tx_power_dbm == TB_LOW_TX_OUTPUT_POWER && lora_data_rate == MAX_LORA_DR){ // completed all PWR OUTPUT AND DR CHANGES
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

	APP_LOG(TS_ON, VLEVEL_L, "Master Rx start\n\r");
	            Radio.Rx(RX_TIMEOUT_VALUE);
}

static void PingPong_Process(void)
{
  Radio.Sleep();

  switch (State)
  {
    case RX:

      if (isMaster == true)
      {
        if (RxBufferSize > 0)
        {
          if (strncmp((const char *)BufferRx, PONG, sizeof(PONG) - 1) == 0)
          {
            UTIL_TIMER_Stop(&timerLed);
            /* switch off green led */
            //HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
            /* master toggles red led */
            //HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); /* LED_RED */
            /* Add delay between RX and TX */
            HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
            /* master sends PING*/
            APP_LOG(TS_ON, VLEVEL_L, "..."
                    "PING"
                    "\n\r");
            APP_LOG(TS_ON, VLEVEL_L, "Master Tx start\n\r");
            memcpy(BufferTx, PING, sizeof(PING) - 1);
            Radio.Send(BufferTx, PAYLOAD_LEN);
          }
          else if (strncmp((const char *)BufferRx, PING, sizeof(PING) - 1) == 0)
          {
            /* A master already exists then become a slave */
            isMaster = false;
            APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
          else /* valid reception but neither a PING or a PONG message */
          {
            /* Set device as master and start again */
            isMaster = true;
            APP_LOG(TS_ON, VLEVEL_L, "Master Rx start\n\r");
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
        }
      }
      else
      {
        if (RxBufferSize > 0)
        {
          if (strncmp((const char *)BufferRx, PING, sizeof(PING) - 1) == 0)
          {
            UTIL_TIMER_Stop(&timerLed);
            /* switch off red led */
            //HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
            /* slave toggles green led */
            //HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /* LED_GREEN */
            /* Add delay between RX and TX */
            HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
            /*slave sends PONG*/
            APP_LOG(TS_ON, VLEVEL_L, "..."
                    "PONG"
                    "\n\r");
            APP_LOG(TS_ON, VLEVEL_L, "Slave  Tx start\n\r");
            memcpy(BufferTx, PONG, sizeof(PONG) - 1);
            Radio.Send(BufferTx, PAYLOAD_LEN);
          }
          else /* valid reception but not a PING as expected */
          {
            /* Set device as master and start again */
            isMaster = true;
            APP_LOG(TS_ON, VLEVEL_L, "Master Rx start\n\r");
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
        }
      }
      break;
    case TX:
      APP_LOG(TS_ON, VLEVEL_L, "Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      break;
    case RX_TIMEOUT:
    case RX_ERROR:
      if (isMaster == true)
      {
        /* Send the next PING frame */
        /* Add delay between RX and TX*/
        /* add random_delay to force sync between boards after some trials*/
        HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN + random_delay);
        APP_LOG(TS_ON, VLEVEL_L, "Master Tx start\n\r");
        /* master sends PING*/
        memcpy(BufferTx, PING, sizeof(PING) - 1);
        Radio.Send(BufferTx, PAYLOAD_LEN);
      }
      else
      {
        APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");
        Radio.Rx(RX_TIMEOUT_VALUE);
      }
      break;
    case TX_TIMEOUT:
      APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      break;
    default:
      break;
  }
}

static void OnledEvent(void *context)
{
  //HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /* LED_GREEN */
  //HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); /* LED_RED */
  UTIL_TIMER_Start(&timerLed);
}

static void Tb_OnTimerRxSynch(void *context){

}
/* USER CODE END PrFD */
