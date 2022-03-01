/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_audio.h
* By      : kevin
* Version : V1.00
* Date    : 2009-4-6 18:34
*********************************************************************************************************
*/
#ifndef _MOD_AUDIO_H_
#define _MOD_AUDIO_H_

#include "typedef.h"
#include <kconfig.h>


#define AUDIO_DEVICE_VOLUME_MIN     (0)      /*  PLAY ��Ƶ�豸������Сֵ               */

#ifdef CONFIG_PLAN_SPRITE
#define AUDIO_DEVICE_VOLUME_MAX     (12)    /*  PLAY ��Ƶ�豸�������ֵ��ԭ�ͻ���              */
#else
#define AUDIO_DEVICE_VOLUME_MAX     (60)    /*  PLAY ��Ƶ�豸�������ֵ�������壩               */
#endif

#define AUDIO_MIC_VOLUME_MIN        (0)      /* ��Ƶ�豸������Сֵ               */
#define AUDIO_MIC_VOLUME_MAX        (25)    /* ��Ƶ�豸�������ֵ               */


#ifndef O_RDONLY
#define O_RDONLY        00000000
#endif
#ifndef O_WRONLY
#define O_WRONLY        00000001
#endif
#ifndef O_RDWR
#define O_RDWR          00000002
#endif
#ifndef O_CREAT
#define O_CREAT         00000100  /* not fcntl */
#endif

typedef enum __AUDIO_DEV_CMD_SET
{
    AUDIO_DEV_CMD_NONE = 0,         /* ��Ƶ�豸�����������Ӧ                 */
    AUDIO_DEV_CMD_START,            /* ������Ƶ�豸                             */
    AUDIO_DEV_CMD_STOP,             /* ֹͣ��Ƶ�豸                             */
    AUDIO_DEV_CMD_PAUSE,            /* ��ͣ��Ƶ�豸                             */
    AUDIO_DEV_CMD_CONTINUE,         /* ������Ƶ�豸                             */

    AUDIO_DEV_CMD_GET_SAMPCNT = 20, /* ��ȡ��Ƶ�Ĳ�������, return = sample count    */
    AUDIO_DEV_CMD_SET_SAMPCNT,      /* ������Ƶ�Ĳ�������, aux = sample count       */
    AUDIO_DEV_CMD_GET_PARA,         /* �����û�����, pbuffer = (__audio_dev_para_t *)   */
    AUDIO_DEV_CMD_SET_PARA,         /* ֧�ֻ���İ汾�Ľӿ���Ч�������ʸ����ⲿ���ڲ��ŵĸ������仯����ȡ�û�����, pbuffer = (__audio_dev_para_t *)   */
    AUDIO_DEV_CMD_GET_VOLUME,       /* ��ȡ����, pbuffer = __audio_device_type_t, return = volume                   */
    AUDIO_DEV_CMD_SET_VOLUME,       /* ��������, aux = volume, pbuffer = __audio_device_type_t, return = new volume */

    AUDIO_DEV_CMD_REG_USERMODE = 40, /* ��Ƶ�û�ע���û�ģʽ, aux = __audio_play_user_t / __audio_rec_user_t */
    AUDIO_DEV_CMD_CHANGE_IF,        /* �û��ı䲥��ģʽ��aux = AUDIO_DEV_IF_IIS, AUDIO_DEV_IF_CODEC */
    AUDIO_DEV_CMD_FLUSH_BUF,        /* �����Ƶ�豸�û��������ڵ���Ƶ����           */
    AUDIO_DEV_CMD_QUERY_BUFSIZE,    /* ��ѯ�û���������ؿռ����,aux = __audio_dev_query_buf_size_type_t   */
    AUDIO_DEV_CMD_RESIZE_BUF,       /* ����ָ���û��������Ĵ�С, aux = buffer size, be careful for use      */
    AUDIO_DEV_CMD_CLOSE_DEV,        /* �ر����豸,��ر�Ӳ���ӿڣ����ã�����, aux = __audio_device_type_t   */
    AUDIO_DEV_CMD_READ_DATA,        /* ����Ƶ�豸��ȡ���ݣ�aux=data size, pbuffer=buffer    */
    AUDIO_DEV_CMD_WRITE_DATA,       /* ����Ƶ�豸д�����ݣ�aux=data size, pbuffer=buffer    */
    AUDIO_DEV_CMD_DATA_FINISH,      /* ֪ͨ��Ƶ�豸��ǰ�û��������Ѿ�������� */

    AUDIO_DEV_CMD_SET_PROTECT_VOL = 60, /* ������������ֵ, aux = max volume value,(0-16)        */
    AUDIO_DEV_CMD_GET_PROTECT_VOL,  /* ��ȡ��������ֵ, return = current max volume value    */

    AUDIO_DEV_CMD_SET_CHAN_MODE,    /* ��������ģʽ(���ҡ�������), aux = __audio_dev_chan_mode_t, return = EPDK_OK/EPDK_FAIL  */
    AUDIO_DEV_CMD_GET_CHAN_MODE,    /* ��ȡ����ģʽ, return = __audio_dev_chan_mode_t               */

    AUDIO_DEV_CMD_ENTER_STANDBY = 80, /* ���� standbyģʽ       */
    AUDIO_DEV_CMD_EXIT_STANDBY,     /* �˳� standbyģʽ       */

    AUDIO_DEV_CMD_REG_CALLBACK = 90, /* ע��һ���ص�����       */
    AUDIO_DEV_CMD_UNREG_CALLBACK,   /* ж��һ���ص�����       */
    AUDIO_DEV_CMD_GET_INTERFACE,    /* ȡ�õ�ǰ��Ƶ���ŵĽӿ�  return = __audio_dev_interface_t */

    AUDIO_DEV_CMD_SET_PLAYMODE = 100, /* ���ṩ��RECʹ�ã�����¼��ʱ���Ƿ���Ҫͬʱ���ţ�Ĭ�ϲ�����, aux = __audio_dev_rec_mode_t        */
    /* ���ṩ��PLAYʹ�ã�Ŀǰ����SPDIFģʽ���Ƿ���RAWDATAģʽ     aux = __audio_dev_spdif_playmode_t  */

    AUDIO_DEV_CMD_SET_DAC_MAX_GAIN = 110,//use "B:\\AUDIO\\PLAY" to set maximum value of Power Amplifier gain to @aux, must <=63
    AUDIO_DEV_CMD_GET_DAC_MAX_GAIN,      //get maximum value of Power Amplifier gain with "B:\\AUDIO\\CTRL"

    AUDIO_DEV_CMD_SET_SW_VOL_MAX,      //use "B:\\AUDIO\\CTRL" to change maximum value of software volume to @aux, this value is the maximum value for end users
    AUDIO_DEV_CMD_GET_SW_VOL_MAX,      //get maximum value of software volume with "b:\\audio\\rec", this value is the maximum value which end users use

    AUDI_DEV_CMD_SET_HPCOM_DRIVE_MODE,      //set HeadPhone driver mode to @aux,  0 means AC mode, 1 means DC mode, other values are reserved(use "B:\\AUDIO\\CTRL")
    AUDIO_DEV_CMD_SWAP_OUTPUT_CHANNELS,     //Swap output channels, if @aux is 1, left dac output to right channel and right dac to left channel. if 0, not swap(use "B:\\AUDIO\\CTRL")

    AUDIO_DEV_CMD_MUTE,//aux: 0: not mute, 1:mute
    AUDIO_DEV_CD_SET_USE_USER_VOLUME_MAP,   /*ʹ���û���������ӳ��pbuffer = ����ΪAUDIO_DEVICE_VOLUME_MAX+1�����飬�����±�������value,Ŀǰ��֧��record����ӳ��*/

    AUDIO_DEV_CMD_SET_MIC_PRE_GAIN,     //set MIC pregain val: 0<=aux<=3 0db 35db 38db 41db   default:41db
    AUDIO_DEV_CMD_MIC_PRE_GAIN_ONOFF,   //MIC pregain onoff: aux=0:off  aux=1:on     default:1
    AUDIO_DEV_CMD_GET_MUTE,             //return: 0: not mute, 1:mute

    AUDIO_DEV_CMD_ENABLE_ECHO = 160,    /*�����Ƿ�֧�ֻ��죬aux:Ϊ0������֧�֣�Ϊ1����֧�֣�Ĭ��֧��*/
    AUDIO_DEV_CMD_SET_ECHO_BUF_MODE,    /*����BUFFERģʽ��aux:0���������Զ����buffer(¼��Ӧ��)��Ϊ1�����Զ����buffer(��¼����Ӧ��)��Ĭ�ϲ����Զ����*/
    AUDIO_DEV_CMD_SET_ECHO_REC_GAIN,    /*(-20, 20)*/
    AUDIO_DEV_CMD_GET_ECHO_REC_GAIN,
    AUDIO_DEV_CMD_SET_ECHO_DELAY_NUM,   /*aux: 0<= aux <= 10 ���û�����ʱ������Ĭ��Ϊ3*/
    AUDIO_DEV_CMD_SET_ECHO_DELAY_TIME,    /*aux:10<= aux <= 3000,Ĭ��Ϊ100���Ժ���Ϊ��λ*/
    AUDIO_DEV_CMD_ECHO_SET_PITCH_MODE,  /*�ýӿ���ʱ���ã�CPU�ܲ��������ñ��ģʽ:
                                        aux: 0xff:�û�ģʽ��1:��Ů�� 2:������ 3:��ͯ��
                                        pbuffer:���aux=0xff,��ôpbufferΪ����ȼ�-8<=pbuffer<=8, ���aux!=0xff����ôpbuffer��Ч*/

    AUDIO_DEV_CMD_ENABLE_ECHO_CANCEL,  /*��������ʹ�ܣ�aux,0:�ر� 1:��*/
    AUDIO_DEV_CMD_ENABLE_ECHO_OUTPUT,  /*ʹ�ܻ��������aux��0:�رջ��������1:�򿪻������*/
    AUDIO_DEV_CMD_ENABLE_RECORD_ECHO,  /*¼��¼�����������Ƿ�����죬aux��0:�������죬1:�����죬Ĭ��Ϊ0*/
    AUDIO_DEV_CMD_SET_SW_GAIN,
    AUDIO_DEV_CMD_GET_SW_GAIN,
    AUDIO_DEV_CMD_ECHO_SET_FIXED_FS,   /*���û��������, aux=48000/44100/24000/22050...���ṩ�ڲ�ͬ���������ò�ͬ�����ʵ���Ҫ�������ʸ���ռ��CPU��Դ�࣬¼�������Ч����*/
    AUDIO_DEV_CMD_DV_MODE,              /*����DVģʽ��aux:1����DV��0������DV*/
    AUDIO_DEV_CMD_FORCE_STOP,           /*ǿ��ֹͣ¼���������AUDIO_DEV_CMD_STOP������ֹͣ¼��DMA��������Դδ�ͷ�*/

    AUDIO_DEV_CMD_PLAY_AUX_WAV_FILE = 200,      /*��������Ƶ���ŵ�ʱ�򲥷�һЩ���ӵ�wav�������ݣ������ݸ�ԭ��Ƶ���ݻ���, aux=__audio_play_aux_wav_mode_t, pbuffer=full filename */
    AUDIO_DEV_CMD_PLAY_AUX_WAV_BUFFER,          /*����ͬ AUDIO_DEV_CMD_PLAY_AUX_WAV_FILE ��������һ����aux=__audio_play_aux_wav_mode_t, pbuffer=(__cedar_pcm_info_t*) */
    AUDIO_DEV_CMD_GET_AUX_WAV_BUFFER_SIZE,   /*��ȡ�������ݲ���buffer��С, ioctrlֱ�ӷ���buffer size*/
    AUDIO_DEV_CMD_SET_AUX_WAV_BUFFER_SIZE,    /*�����������ݲ���buffer��С, aux=buffer size*/
    AUDIO_DEV_CMD_GET_AUX_WAV_DATA_SIZE,      /*��ȡ�������ݻ�������Ч���ݳ���*/
    AUDIO_DEV_CMD_FOURCE_SET_PARA,            /*ǿ�����ò����ʣ�����aitalkӦ�ã�Ӧ��Ҫ������ʱ���Ϊ16k*/
    AUDIO_DEV_CMD_SET_UNRUN_SAMPLE_SEL,       /*û��audio���ݵ�ʱ���Ƿ���: aux:0 mute ,  1 not mute*/
    AUDIO_DEV_CMD_PRINT_REGS,                 /*��ӡDA��ؼĴ���*/
    AUDIO_DEV_CMD_START_DUMP_PLAY_DAT,
    AUDIO_DEV_CMD_STOP_DUMP_PLAY_DAT,
    AUDIO_DEV_CMD_SET_SPSC_REC_USR_MODE,   /* aux = 1����Ϊ����ok audio rec*/
    AUDIO_DEV_CMD_SET_LR_CHANNEL,           /*������������ģʽ��aux:0������1:������������*/
    AUDIO_DEV_CMD_ENABLE_REC_LOW_PASS,          /*ʹ�ܵ�ͨ�˲���auxΪ1�����򿪣�Ϊ0�����ر�*/

    AUDIO_DEV_CMD_DRAIN,                    /*�Ȼ��������ݲ���*/

#ifdef CONFIG_SOC_SUN3IW1
    AUDIO_DEV_CMD_SET_LR_EN_BITS,           /*����DAC����ͨ����ʹ��(Ĭ������ͨ������)��aux:0������������1������������pbuffer:0�����رգ�1������*/

    AUDIO_DEV_CMD_GET_REC_AVE_VAL = 240,  /*��ȡ����ƽ����С*/
    AUDIO_DEV_CMD_SONIC_INIT,
    AUDIO_DEV_CMD_SONIC_EXIT,
    AUDIO_DEV_CMD_SONIC_SET_CB, //���ûص����������յ�����ʱ��ʲô����
    AUDIO_DEV_CMD_SONIC_SET_FAIL_CB, //���ûص����������յ���������ʱ��ʲô����


    AUDIO_DEV_CMD_MAGIC_STOP_SET_CB,
    AUDIO_DEV_CMD_MAGIC_START_SET_CB,
    AUDIO_DEV_CMD_MAGIC_SET_VAL,
    AUDIO_DEV_CMD_MAGIC_SET_START_FLAG,
    AUDIO_DEV_CMD_MAGIC_GET_CIRBUF_AND_SIZE,
    AUDIO_DEV_CMD_MAGIC_FLUSH_CIRCLY_BUF,
    AUDIO_DEV_CMD_EXIT,




    //���¼����ӿ���Ҫ��������ҪADC����ͨ��I2S��������
    AUDIO_DEV_RECORD_SET, //��Ҫ�򿪵���"AUDIO//REC"
    //ֻ¼MIC or FM or LINEIN��һ�������������Ҫ���ã�¼��MIC�ͱ������ֽ������������ʱ����
    //aux=0, pbuffer=0   : MIC+��������¼�ƹ�
    //aux=1, puffer=0:   :MIC+��������¼�ƿ�
    //aux=0, pbuffer=1     : MIC+FM¼�ƹ�
    //aux=1, puffer=1:     :MIC+FM¼�ƿ�
    //aux=0, pbuffer=2     : MIC+LINEIN¼�ƹ�
    //aux=1, puffer=2:     :MIC+LINEIN¼�ƿ�

    AUDIO_DEV_CMD_SET_REC_OUTPUT_FLAG, //��Ҫ�򿪵���""b:\\AUDIO\\CTRL""
    //aux=0:¼��ֻ¼�������  aux=1:ADC¼����ͬʱҲ���DAC or I2S

    AUDIO_DEV_CMD_SET_OUTPUT_MODE,//��Ҫ�򿪵���""b:\\AUDIO\\CTRL""
    //aux=0:ֻ���DAC  1:ֻ���i2s  2:dac+i2s

    AUDIO_DEV_CMD_SET_REC_MODE,//��Ҫ�򿪵���""b:\\AUDIO\\CTRL""
    //0:Ĭ�ϻ�д��MIC�� pre_buf ��ginkgo¼��  1:����Ҫ��д��MIC��pre_Buf


    AUDIO_DEV_CMD_SET_PATH_GAIN,//aux = 0: mic+����   aux=1:fm+mic or linein+mic



    AUDIO_DEV_CMD_GET_PCM_PITCH,//��Ҫ�򿪵���""b:\\AUDIO\\CTRL""
    AUDIO_DEV_CMD_GET_PCM_BUFFER,//��Ҫ�򿪵���""b:\\AUDIO\\CTRL""

    AUDIO_DEV_CMD_FM_CHANS_SEL,//��Ҫ�򿪵���""b:\\AUDIO\\CTRL""       aux:1 ->left  2->right 3->left &right   pbuffer:0 :off  1:on

#endif
} __audio_dev_cmd_set_t;


typedef enum __AUDIO_DEVICE_TYPE
{
    AUDIO_DEV_NONE = 0,         /* δ������Ƶ�豸                               */
    AUDIO_DEV_PLAY,             /* ��Ƶ�����豸                                 */
    AUDIO_DEV_REC,              /* ��Ƶ�����豸                                 */
    AUDIO_DEV_FM,               /* FM�豸                                       */
    AUDIO_DEV_

} __audio_device_type_t;


typedef enum __AUDIO_PLAY_USER
{
    AUDIO_PLAY_USR_NONE = 0,    /* ��Ƶ�����û�δ����ģʽ                       */
    AUDIO_PLAY_USR_MASTER,      /* �û�ʹ��masterģʽ������Ƶ                   */
    AUDIO_PLAY_USR_SLAVE,       /* �û�ʹ��slaveģʽ������Ƶ                    */
    AUDIO_PLAY_USR_KEY,         /* �û�ʹ��key tone������Ƶ                     */
    AUDIO_PLAY_USR_

} __audio_play_user_t;

typedef enum __AUDIO_REC_USER
{
    AUDIO_REC_USR_NONE = 0,    /* ¼���û�δ����ģʽ                           */
    AUDIO_REC_USR_LINEIN,      /* ¼���û�LINE INģʽ                          */
    AUDIO_REC_USR_FMIN,        /* ¼���û�FM INģʽ                            */
    AUDIO_REC_USR_MIC,         /* ¼���û�MICģʽ                              */
    AUDIO_REC_USR_MIXER,       /* ¼���û�MIXERģʽ                            */
    AUDIO_REC_USR_
} __audio_rec_user_t;


#ifdef CONFIG_SOC_SUN3IW2
typedef enum __AUDIO_FM_MODE
{
    AUDIO_FM_MODE_LINEIN,
    AUDIO_FM_MODE_FMIN,
    AUDIO_FM_MODE_
} __audio_fm_mode;
#endif

typedef enum __AUDIO_CHN_MODE
{
    AUDIO_CHN_MODE_NONE = 0,    /* ��Ƶͨ��ģʽδ����                           */
    AUDIO_CHN_MODE_MONO,        /* ��ͨ��ģʽ                                   */
    AUDIO_CHN_MODE_STEREO,      /* ������ģʽ                                   */
    AUDIO_CHN_MODE_

} __audio_chn_mode_t;


//�û��������ݽṹ
typedef struct __AUDIO_DEV_PARA
{
    __u8        chn;            /* ��Ƶͨ����, __audio_chn_mode_t               */
    __u8        bps;            /* ��Ƶ������λ���� 8/16/24                     */
    __u32       fs;             /* ��Ƶ������                                   */

} __audio_dev_para_t;


typedef struct __AUDIO_IIS_TO_HDMI
{
    __u16  mclk_div;            /* Mclock��Ƶϵ��                               */
    __u8   channel;             /* ��Ƶͨ����                                   */
    __u8   bitpersample;        /* ��Ƶ������λ��                               */

} __audio_iis_to_hdmi_t;


typedef enum __AUDIO_DEV_INTERFACE
{
    AUDIO_DEV_IF_NONE = 0,      /* δ������Ƶ�豸�ӿ�                           */
    AUDIO_DEV_IF_CODEC,         /* ����audio codec��Ϊ��Ƶ�豸�ӿ�              */
    AUDIO_DEV_IF_IIS,           /* ����IIS������Ϊ��Ƶ�豸�ӿ�                  */
    AUDIO_DEV_IF_SPDIF,         /* ????SPDIF??????Ϊ??Ƶ?�?ӿ?                */
    AUDIO_DEV_IF_CODEC_IIS,    /*audio codec and iis ͬʱ????*/
    AUDIO_DEV_INTERFACE_
} __audio_dev_interface_t;

typedef enum __AUDIO_DEV_QUERY_BUF_SIZE_TYPE
{
    AUDIO_DEV_QUERY_BUF_SIZE_NONE = 0,      /* δ����Ҫ��ȡ�ռ������           */
    AUDIO_DEV_QUERY_BUF_SIZE_DATA,          /* ��ȡ�������ڵ�����size           */
    AUDIO_DEV_QUERY_BUF_SIZE_FREE,          /* ��ȡ�������ڵĿ��пռ�size       */
    AUDIO_DEV_QUERY_BUF_SIZE_TOTAL,         /* ��ȡ�������ܿռ�                 */

    AUDIO_DEV_QUERY_BUF_SIZE_

} __audio_dev_query_buf_size_type_t;


typedef enum __AUDIO_DEV_CHAN_MODE
{
    AUDIO_DEV_CHANNEL_STEREO = 0,           /* ������ģʽ                       */
    AUDIO_DEV_CHANNEL_LEFT,                 /* ����ģʽ                         */
    AUDIO_DEV_CHANNEL_RIGHT,                /* ����ģʽ                         */
    AUDIO_DEV_CHANNEL_EXIT,
    AUDIO_DEV_CHANNEL_

} __audio_dev_chan_mode_t;

typedef enum __AUDIO_DEV_REC_PLAYMODE
{
    AUDIO_DEV_REC_NOPLAY = 0,               /* ¼��ʱ����Ҫ����               */
    AUDIO_DEV_REC_WITHPLAY,                 /* ¼��ʱ����Ҫ����                 */

    AUDIO_DEV_REC_

} __audio_dev_rec_playmode_t;


#ifdef CONFIG_SOC_SUN3IW1
typedef enum __AUDIO_DEV_SPDIF_PLAYMODE
{
    AUDIO_DEV_SPDIF_NORMALPLAY = 0,         /* SPDIFʱ?򲥷Ž???????            */
    AUDIO_DEV_SPDIF_RAWDATAPLAY,            /* SPDIFʱ?򲥷?RWDATA????          */
    AUDIO_DEV_SPDIF_
} __audio_dev_spdif_playmode_t;
#endif

typedef struct __AUDIO_PCM_INFO
{
    //input infor
    __s32   SampleRate;
    __u16  *PCMPtr;
    __s32   PcmLen;//sample num;    inputA��inputB�ڵ���ǰ��Ҫmix��sample��,���ú���ʣ��û��mix��sample��.
    __s32   Chan;  //channel num
    //reserved
    int     preamp;//-20 -- 20 db
} __audio_pcm_info_t;

#ifdef CONFIG_SOC_SUN3IW1
typedef struct sonic_index
{
    __u16    index1; //һ???˵?
    __u16    index2;//?????˵?
} sonic_index_t;

typedef struct pcm_buffer_s
{
    __u8 *pbuf;
    __s32 size;
} pcm_buffer_t;
#endif

//����AUDIO_PLAY_AUX_WAV_FILE��ģʽ
typedef enum __AUDIO_PLAY_AUX_WAV_MODE
{
    AUDIO_PLAY_AUX_WAV_MODE_APPEND,//�����ǰ�Ѿ���δ������ϵ�AUX WAV���ݣ���ǰ����׷�ӵ������ݵĺ��沥��
    AUDIO_PLAY_AUX_WAV_MODE_TRUNCATE,//�����ǰ�Ѿ���δ������ϵ�AUX WAV���ݣ��������ǰ���ݣ����ŵ�ǰ���õ�����
    AUDIO_PLAY_AUX_WAV_MODE_MIX,//�����ǰ�Ѿ���δ������ϵ�AUX WAV���ݣ�����ǰ�����ݺ͵�ǰ���õ����ݽ��л���
    AUDIO_PLAY_AUX_WAV_MODE_IGNORE,//�����ǰ�Ѿ���δ������ϵ�AUX WAV���ݣ�����Ե�ǰ���õ�����
    AUDIO_PLAY_AUX_WAV_MODE_MAX
} __audio_play_aux_wav_mode_t;

typedef enum
{
    AUDIO_REC_FM_L     = (1<<0),//����ͬʱʹ��FM��LINEIN
    AUDIO_REC_FM_R     = (1<<1),
    AUDIO_REC_LINEIN_L = (1<<2),
    AUDIO_REC_LINEIN_R = (1<<3),
    AUDIO_REC_MIC      = (1<<4),
}__audio_record_channel_t;

#endif /* _DRV_AUDIO_H_ */