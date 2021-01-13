/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//  *** GAME TERMINAL SIZE IS 80x26 ***

int bx = 10; //bird x-axis position
int by = 10; //bird y-axis position
int map[25][80]; //map high25xlong80
int frame = 0x58; //"X"
int ob = 0x23;// "#"
int bird = 0x4f;// "O"
int space = 0x20; //" "
int oby[5]; // obstacle y-axis position
int obx[5] = {16,32,48,64,80}; //starting obstacle x-axis position
int score = 0; //your score
char charScore[16];
int alive = 0; //1 = the bird is alive, will be 1 after press start
int kb; //keyboard input
int pause = 0; //1 = game pause
int fly = 0; // times of blue button push

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //push button interrupt pleae push the button firmly
{
	if(alive && !pause)
	{
		if(by-fly>1) //bird fly
		{
			if(fly==0)
			{
				fly++;
			}
			fly++;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //receive kb interrupt
{

	if(kb == 0x70 && alive )// 0x70 = "p" = pause game
	{
		pause = 1;
		pauseGame();
	}
	else if(kb == 0x70) //"p" = play game
	{
		  for (int i=0;i<5;i++) //random oby first time
		  {
			  oby[i]=rand()%15+5;
		  }
		  alive = 1;
	}
	else if(kb == 0x68 && !alive)//press h to how to play
	{
		clrscr();
		homescr();
		HAL_UART_Transmit(&huart2,
				"\n\n\n"
				"                   +---+---+---+ +---+---+ +---+---+---+---+                    "
				"                   | H | O | W | | T | O | | P | L | A | Y |                    "
				"                   +---+---+---+ +---+---+ +---+---+---+---+                    "
				"\n\n\n"
				"                                YOU ARE A BIRD.                                 "
				"\n"
				"          You have to fly through the obstacles, as many as possible.           "
				"                    If you hit an obstacle, the game is over                    "
				"\n\n"
				"                          Fly by pressing blue button.                          "
				"              While playing, you can pause the game by pressing p.              "
				"            Restart the game by pressing reset button (black button)            "
				"\n\n\n"
				"                             +-------------------+                              "
				"                             |  Press p to play  |                              "
				"                             +-------------------+                              ", 960, HAL_MAX_DELAY);
	 }
	else
	{
		pause = 0;
	}
	HAL_UART_Receive_IT(&huart2, &kb,  1); //restart receiving kb interrupt
}

void pauseGame() //when game pause
{
	kb = 0x20;
	while(kb != 0x70)
	{
		clrscr();
		homescr();
		HAL_UART_Transmit(&huart2,
				"\n\n\n\n"
				"                   e88'Y88      e Y8b         e   e     888'Y88                 "
				"                  d888  'Y     d8b Y8b       d8b d8b    888 ,'Y                 "
				"                 C8888 eeee   d888b Y8b     e Y8b Y8b   888C8                   "
				"                  Y888 888P  d888888888b   d8b Y8b Y8b  888 \",d                 "
				"                   \"88 88\"  d8888888b Y8b d888b Y8b Y8b 888,d88                 "
				"\n\n"
				"                  888 88e      e Y8b     8888 8888  dP\"8 888'Y88                "
				"                  888 888D    d8b Y8b    8888 8888 C8b Y 888 ,'Y                "
				"                  888 88\"    d888b Y8b   8888 8888  Y8b  888C8                  "
				"                  888       d888888888b  8888 8888 b Y8D 888 \",d                "
				"                  888      d8888888b Y8b 'Y88 88P' 8edP  888,d88                 ", 800, HAL_MAX_DELAY);
		//text credit: https://www.kammerl.de/ascii/AsciiSignature.php , font: rozzo
		HAL_UART_Transmit(&huart2, "\n\n\n\n        ", 10, 10);
		HAL_UART_Transmit(&huart2,
				"                            +-----------------------+                           "
				"                            |  Press p to continue  |                           "
				"                            +-----------------------+                           ", 240, HAL_MAX_DELAY);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		HAL_Delay(500);
		HAL_UART_Receive(&huart2, &kb, 1, 10);
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
	pause = 0;
	clrscr();
}

void createMap() //draw the map
{
	homescr();
	for (int i=0;i<25;i++) //draw the map high25xlong80
	{
		for (int j=0;j<80;j++)
		{
			if ( i==0 || i==24|| j==79 || j==0)
			{
				map[i][j]= frame;
			}
	        else
	        {
	        	map[i][j]= space;
	        }
		}
	}
	for (int i =0;i<5;i++) //draw obstacle
	{
		for (int j=1;j<24;j++)
		{
			map[j][obx[i]] = ob;
		}
	}
	for (int i=0;i<5;i++) //draw obstacle's hole
	{
		for(int n=0;n<4;n++)
		{
			map[oby[i]+n][obx[i]] = space;
		}
	}
	for (int i=0;i<25;i++) //draw the bird
	{
		for (int j=0;j<80;j++)
		{
			if( bx == j && by == i) //draw bird
			{
				map[i][j] = bird; //"O"
			}
	    }
	}
	HAL_UART_Transmit(&huart2, map, sizeof(map), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "                                YOUR SCORE = ", 45, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)charScore, sprintf(charScore, "%d", score), HAL_MAX_DELAY);
}

void moveOb()
{
	for (int i=0;i<5;i++) //move obstacle to the right
	{
		obx[i]--;
	}
	for (int i=0;i<5;i++)
	{
		if (obx[i]==0)
		{
			obx[i]=78;
			oby[i]= rand()%15+5;
		}
	}
}

void pointOrDie()
{
	for(int i=0;i<5;i++)
	{
		if(obx[i]==10)
		{
			if(map[by][10] == space)
				score++;
		    if(map[by][10] == ob) //bird hit the obstacle
		       	alive=0; //bird die, game over
		}
	}
}

void clrscr() // Clear the screen
{
	char clrscr[] = {0x1B, '[', '2' , 'J',0};
	HAL_UART_Transmit(&huart2, clrscr, sizeof(clrscr), HAL_MAX_DELAY);
}

void homescr() // Home the cursor
{
	char homescr[] = {0x1B, '[' , 'H' , 0};
	HAL_UART_Transmit(&huart2, homescr, sizeof(homescr), HAL_MAX_DELAY);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2, &kb, 1); //start receiving kb interrupt
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  clrscr();
  homescr();
  HAL_UART_Transmit(&huart2,
    	  "                                                                                "
  		  "                                       _.------.                                "
  		  "                                  _ _.-`    (\'>.-`\"""-.                         "
  		  "                          \'.__.---\'       _\'`   _ .--.)                         "
  		  "                                 -\'         \'-.-\';`   `                         "
  		  "                                 \' -      _.\'  ``\'--.                           "
  		  "                                     '---`    .-\'\"\"`                            "
  		  "                                            /`\"                                   ", 642, HAL_MAX_DELAY);
    //art credit: https://www.asciiart.eu/ , Eagle/falcon by Joan Stark
  HAL_Delay(400);
  HAL_UART_Transmit(&huart2,
          "                                                                                "
  		  "    888\'Y88 888         e Y8b     888 88e  888 88e  Y88b Y8P                    "
  		  "    888 ,\'Y 888        d8b Y8b    888 888D 888 888D  Y88b Y                     "
  		  "    888C8   888       d888b Y8b   888 88\"  888 88\"    Y88b                      "
  		  "    888 \"   888  ,d  d888888888b  888      888         888                      "
  		  "    888     888,d88 d8888888b Y8b 888      888         888                      "
    	  "                                                                              ", 560, HAL_MAX_DELAY);
  HAL_Delay(200);
  HAL_UART_Transmit(&huart2,
   		  "                                               888 88b, 888 888 88e  888 88e    "
   		  "                                               888 88P' 888 888 888D 888 888b   "
   		  "                                               888 8K   888 888 88\"  888 8888D  "
   		  "                                               888 88b, 888 888 b,   888 888P   "
   		  "                                               888 88P' 888 888 88b, 888 88\"    ", 400, HAL_MAX_DELAY);
    //text credit: https://www.kammerl.de/ascii/AsciiSignature.php , font: rozzo
  HAL_UART_Transmit(&huart2, "\n", 1, HAL_MAX_DELAY);
  HAL_Delay(200);
  HAL_UART_Transmit(&huart2,
		  "                         +---------------------------+                          "
		  "                         |       Play (Press p)      |                          "
  	  	  "                         |   How to play (Press h)   |                          "
		  "                         +---------------------------+                          ", 320, HAL_MAX_DELAY);
  while(!alive)
  {
	  //wait until player press p
  }
  while(alive) //game start
  {
	  HAL_UART_Receive_IT(&huart2, &kb,  1);
	  if(!pause)
	  {
		  moveOb();
	  }
	  createMap();
	  if(!pause)
	  {
		  if(by<23)
		  {
			  by++; //the bird fly falls by gravity
		  }
		  by-=fly; //the bird fly up by user push blue button
		  fly=0;//reset
		  pointOrDie();

	  }
  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  //game over
  HAL_Delay(500);
  clrscr();
  homescr();
  HAL_UART_Transmit(&huart2,
		  "\n\n\n\n"
		  "                   e88'Y88      e Y8b         e   e     888'Y88                 "
		  "                  d888  'Y     d8b Y8b       d8b d8b    888 ,'Y                 "
		  "                 C8888 eeee   d888b Y8b     e Y8b Y8b   888C8                   "
		  "                  Y888 888P  d888888888b   d8b Y8b Y8b  888 \",d                 "
		  "                   \"88 88\"  d8888888b Y8b d888b Y8b Y8b 888,d88                 ", 400, HAL_MAX_DELAY);
  HAL_Delay(300);
  HAL_UART_Transmit(&huart2,
		  "\n\n   "
		  "                       e88 88e   Y8b Y88888P 888'Y88 888 88e                    "
		  "                      d888 888b   Y8b Y888P  888 ,'Y 888 888D                   "
		  "                     C8888 8888D   Y8b Y8P   888C8   888 88\"                    "
		  "                      Y888 888P     Y8b Y    888 \",d 888 b,                     "
		  "                       \"88 88\"       Y8P     888,d88 888 88b,                   ", 403, HAL_MAX_DELAY);
  HAL_Delay(300);
  HAL_UART_Transmit(&huart2, "\n\n", 2, HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2, "                                     YOUR SCORE = ", 50, HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2, (uint8_t*)charScore, sprintf(charScore, "%d", score), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2,
		  "\n                                "
		  "               +------------------------------------------------+               "
		  "               |  Press reset button (black button) to restart  |               "
		  "               +------------------------------------------------+               ", 273, HAL_MAX_DELAY);
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
