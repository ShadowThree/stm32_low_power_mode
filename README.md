# `STM32F1`�͹���ģʽ
## �ο�
1. `Application Note: AN2629`
2. [���ͣ�`low power mode in stm32`](https://controllerstech.com/low-power-modes-in-stm32/)

## ʹ��
1. �޸�`LOW_POWER_MODE`�궨��Ϊָ���͹���ģʽ��Ȼ�������¼���ɣ�

## ˵��
1. ���ֵ͹���ģʽ�ܽ�
![low power modes](./images/lowPowerMode.png)
> ### Sleep mode
> 1. ��ͨ�������жϻ��ѣ�����ͨ�����������ڽ����жϵȷ�ʽ�����ԣ�Ҳ����ͨ�������¼����ѣ����ⲿ�����Ľ��жϣ��ڲ������Ľ��¼���
> 2. ��Ϊ`sysTick`�ж�Ҳ�ỽ��`sleep mode`����`sysTick`Ĭ��ÿ���붼����һ���жϣ�����Ϊ�˱��ⱻ����жϻ��ѣ�����`sleep`ǰ��Ҫ�ر�`sysTick`��
> 3. ���ڽ���`sleep`�ر���`sysTick`�����Ի���`sleep`����Ҫ����`sysTick`��
> 4. ����`HAL_PWR_EnableSleepOnExit()`����`sleep`��ÿ�λ��Ѻ�ִֻ���ж��еĴ��룬ִ������Զ�����`sleep`���������ж���ͨ��`HAL_PWR_DisableSleepOnExit`�˳���
> 5. ��`Sleep`ģʽ�£�`MCU`���ٹ�����������Թ��������������ǿ��Դ�ӡ�ģ��Ĵ�����`RAM`���棻

> ### Stop mode
> 1. ��ͨ������`EXTI`�жϻ��ѣ�ע���`Sleep`�����𣬴����жϿ��Ի���`Sleep`�����ǲ��ܻ���`Stop`��
> 2. `Stop`ģʽ�£�`HSE`�رգ����Ի��Ѻ���Ҫ����`SystemClock_Config()`��������ϵͳʱ�ӣ�
> 3. ͬ�����롢�˳�`Stop mode`Ҳ��Ҫ�رա�����`sysTick`��
> 4. `Stop mode`Ҳ����ʹ��`HAL_PWR_EnableSleepOnExit()`��
> 5. `Stop`ģʽ�£�`MCU`���ٹ��������ڲ��ܴ�ӡ���Ĵ�����`RAM`�����ݱ��棻

> ### standby mode
> 1. `Standby mode`��ǰ��������һ����ǰ�������͹���ģʽ�����Ѻ󣬿��Խ����ڽ���͹���ģʽ�ĵط����У���`Standby`���Ѻ�Ḵλ`MCU`��ֻ��ͨ��`PWR_FLAG_SB`״̬λ�ж��������ĸ�λ�����������ڻ���`Standby`�����ĸ�λ������
> 2. `Standby`���Ա�`WKUP`�ܽţ�`RTC alarm`��`NRST`�ܽţ�`IWDG reset`���ѣ�
> 3. ���˱��ݼĴ����������Ĵ�����`RAM`��ֵ����ʧ�����Ի��Ѻ����Ҫ������