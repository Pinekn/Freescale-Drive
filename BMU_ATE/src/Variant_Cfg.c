/*************************************************************************
Filename: Variant_Cfg.h

Modification Record:

**************************************************************************/
#include "../inc/Variant_Cfg.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../inc//global.h"
//#include "../Log/net_log.h"


//BMU_BOARD_NO 1
  #define PACKET_CHG_CURR_GAIN_NUM_B1           1016
  #define PACKET_CHG_CURR_GAIN_DENOM_B1       	1000
  #define PACKET_CHG_CURR_OFFSET_B1          	  28

  #define PACKET_DISCHG_CURR_GAIN_NUM_B1        1015
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B1      1000
  #define PACKET_DISCHG_CURR_OFFSET_B1        	28

  #define PACKET_VOLT_OFFSET_NUM_B1             -16

//BMU_BOARD_NO 2
  #define PACKET_CHG_CURR_GAIN_NUM_B2           980
  #define PACKET_CHG_CURR_GAIN_DENOM_B2       	1000
  #define PACKET_CHG_CURR_OFFSET_B2           	25

  #define PACKET_DISCHG_CURR_GAIN_NUM_B2        977
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B2      1000
  #define PACKET_DISCHG_CURR_OFFSET_B2        	25

  #define PACKET_VOLT_OFFSET_NUM_B2             5

//BMU_BOARD_NO 3
  #define PACKET_CHG_CURR_GAIN_NUM_B3           983
  #define PACKET_CHG_CURR_GAIN_DENOM_B3       	1000
  #define PACKET_CHG_CURR_OFFSET_B3           	-5

  #define PACKET_DISCHG_CURR_GAIN_NUM_B3        987
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B3      1000
  #define PACKET_DISCHG_CURR_OFFSET_B3         	48

  #define PACKET_VOLT_OFFSET_NUM_B3             -24

// BMU_BOARD_NO == 4 )
  #define PACKET_CHG_CURR_GAIN_NUM_B4           982
  #define PACKET_CHG_CURR_GAIN_DENOM_B4       	1000
  #define PACKET_CHG_CURR_OFFSET_B4           	20

  #define PACKET_DISCHG_CURR_GAIN_NUM_B4        977
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B4      1000
  #define PACKET_DISCHG_CURR_OFFSET_B4        	20

// BMU_BOARD_NO == 5 )
  #define PACKET_CHG_CURR_GAIN_NUM_B5           978
  #define PACKET_CHG_CURR_GAIN_DENOM_B5       	1000
  #define PACKET_CHG_CURR_OFFSET_B5           	-11

  #define PACKET_DISCHG_CURR_GAIN_NUM_B5        987
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B5      1000
  #define PACKET_DISCHG_CURR_OFFSET_B5        	-11

  #define PACKET_VOLT_OFFSET_NUM_B5             7

// BMU_BOARD_NO == 6
  #define PACKET_CHG_CURR_GAIN_NUM_B6           1009
  #define PACKET_CHG_CURR_GAIN_DENOM_B6       	1000
  #define PACKET_CHG_CURR_OFFSET_B6           	62

  #define PACKET_DISCHG_CURR_GAIN_NUM_B6        995
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B6      1000
  #define PACKET_DISCHG_CURR_OFFSET_B6        	61

  #define PACKET_VOLT_OFFSET_NUM_B6             -2

//  BMU_BOARD_NO == 7
  #define PACKET_CHG_CURR_GAIN_NUM_B7           1004
  #define PACKET_CHG_CURR_GAIN_DENOM_B7       	1000
  #define PACKET_CHG_CURR_OFFSET_B7           	13

  #define PACKET_DISCHG_CURR_GAIN_NUM_B7        1006
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B7      1000
  #define PACKET_DISCHG_CURR_OFFSET_B7        	13

// BMU_BOARD_NO == 8
  #define PACKET_CHG_CURR_GAIN_NUM_B8           979
  #define PACKET_CHG_CURR_GAIN_DENOM_B8       	1000
  #define PACKET_CHG_CURR_OFFSET_B8           	9

  #define PACKET_DISCHG_CURR_GAIN_NUM_B8        978
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B8      1000
  #define PACKET_DISCHG_CURR_OFFSET_B8        	9

// BMU_BOARD_NO == 9 )
  #define PACKET_CHG_CURR_GAIN_NUM_B9           1022
  #define PACKET_CHG_CURR_GAIN_DENOM_B9       	1000
  #define PACKET_CHG_CURR_OFFSET_B9           	22

  #define PACKET_DISCHG_CURR_GAIN_NUM_B9        1008
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B9      1000
  #define PACKET_DISCHG_CURR_OFFSET_B9        	22

  #define PACKET_VOLT_OFFSET_NUM_B9             -42

// BMU_BOARD_NO : 10
  #define PACKET_CHG_CURR_GAIN_NUM_B10          981
  #define PACKET_CHG_CURR_GAIN_DENOM_B10      	1000
  #define PACKET_CHG_CURR_OFFSET_B10          	-30

  #define PACKET_DISCHG_CURR_GAIN_NUM_B10       995
  #define PACKET_DISCHG_CURR_GAIN_DENOM_B10     1000
  #define PACKET_DISCHG_CURR_OFFSET_B10       	-31

/******************************************
No2:
  980,     Packe Charge current gain Of numerator
  1000,    Packe charge current gain of denominator
    25,    Packe charge current offset

  977,     Packe discharge current gain Of numerator
  1000,    Packe discharge current gain of denominator
    25,    Packe discharge current offset

No3:
  1005,    Packe Charge current gain Of numerator
  1000,    Packe charge current gain of denominator
    48,    Packe charge current offset

  1002,    Packe discharge current gain Of numerator
  1000,    Packe discharge current gain of denominator
    45,    Packe discharge current offset

*/


const Bmu_Config_C_Types Bmu_Config_C =
{
  {
    -3, -3  //AFE Offset,
  },

  {
    {1,0,0,0},   //AFE Offset
    {1,0,0,0},
  },

  {
    5000, 5300  //Cell temp vlot referece
  },

  33,              //hystereticThreshold  10mV  ;
  7,               //cellBalanceThreshold  2mV- 7;
  1000,            //onTime_ms;
  30,              //.pulseChargeTime,uint:s
  100,             //.pulseRelaxTime ;uint:0.1s

  2800,            // maxChrgCurrent  //2.5A;118V*2.5A = 295W. signle packet
  2800,            // maxDischrgCurrent.signle packet

};

Bms_ConfigTypes     Bms_ConfigPar;



FILE* cfg = 0;

static int open_exe_cfg(const char* module)
{
  if (cfg) // if the configure is be opne,the return ;
    return 0;

  char exe_name[256];
  // 获得当前运行的程序名
  if (!get_module_file_name(exe_name, 128))
    return -1;

  strcat(exe_name, ".cfg");

  // 打开配置文件
  cfg = fopen(exe_name, module);

  if (!cfg)
    return -1;

  return 0;
}

// 跳过空格
static const char* add_blank(const char* row)
{
  while(*row == ' ' || *row == '\t')
    ++ row;
  return row;
}

// 跳过空格
static const char* sub_blank(const char* row)
{
  while(*row == ' ' || *row == '\t')
    -- row;
  return row;
}

// 查找给定函数变量值
static const char* get_value(const char* funname, const char* var, int v_l, char* val)
{
  const char* offset;
  char row_dat[1024];
  char var_name[64];
  var_name[0] = 0;
  val[0] = 0;
  fseek(cfg, 0, SEEK_SET);
  short status = 0;
  short key_len = strlen(var);
  int row = 0;
  while(!feof(cfg))
  {
    fgets(row_dat, 1024, cfg);
    ++ row;
    offset = row_dat;

    offset = add_blank(offset);

    // # 为注释，跳过本行
    if (*offset == '#' || *offset == '\r' || *offset == '\n')
      continue;

    // 状态表0 找函数名
    if (0 == status)
    {
      const char* p = strstr(offset, funname);
      if (!p)
        continue;

      p = strstr(offset, "()");
      if (!p)
        continue;

      // 函数名找到改变状态
      status = 1;
    }

    // 状态表1 找"{"
    if (1 == status)
    {
      offset = strstr(offset, "{");
      if (!offset)
        continue;

      ++ offset;
      offset = add_blank(offset);
      status = 2;
    }

    // 状态表2 找变量
    if (2 == status)
    {
      // 如果为 "}" 查找失败
      if (*offset == '}')
      {
        return NULL;
      }
      if (*offset == '\r' || *offset == '\n')
        continue;

      // 如果没有找到变时找到'='，语法错误
      if (*offset == '=')
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, not defined variable\033[m\n", row);
        return NULL;
      }

      // 如果变量中出现"() = "没有找到变量
      if (strstr(offset, "()") && !strstr(offset, "="))
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, not ending flag\033[m\n", row);
        break;
      }

      const char* start = offset;
      const char* end = strstr(offset, "=");
      // 没有找到=，出现语法错误
      if (!end)
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, not \"=\" \033[m\n", row);
        return NULL;
      }
      offset = end;

      -- end;
      end = sub_blank(end);

      int var_len = end - start + 1;
      // 变量名过长
      if (var_len >= 64)
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, variable name long\033[m\n", row);
        return NULL;
      }

      if (var_len != key_len)
        continue;

      memcpy(var_name, start, var_len);
      var_name[var_len] = 0;

      if (!strstr(var_name, var))
        continue;

      ++ offset;
      // 跳过空格
      offset = add_blank(offset);

      // 变量名没有值
      if (*offset == '}' || *offset == '\r' || *offset == '\n')
      {
        if (*offset == '}')
        {
          //fprintf(stdout, "\033[31m[error] Config file error line = %d, not ending flag\033[m\n", row);
          return NULL;
        }
        continue;
      }

      // 查到行尾
      end = strstr(offset, "\n");
      if (!end)
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, not ending flag\033[m\n", row);
        return NULL;
      }

      --end;
      if (*end == '\r')
        -- end;
      // 删除空格
      end = sub_blank(end);

      var_len = end - offset + 1;
      if (var_len >= v_l)
      {
        //fprintf(stdout, "\033[31m[error] Config file error line = %d, value long\033[m\n", row);
        return NULL;
      }

      // 考备值
      memcpy(val, offset, var_len);
      val[var_len] = 0;
      status = 3;
      break;
    }
  }

  if (3 != status)
  {
    //fprintf(stdout, "\033[31m[error] Config file error line = %d, funcount \"%s\" not ending flag\033[m\n", row, funname);
    return NULL;
  }
  return val;
}

int open_cfg(const char* filename, const char* module)
{
  if (!filename)
  {
    // 打开当前执行程序名的配置文件
    return open_exe_cfg(module);
  }
  else
  {
    // 打开指定名称的配置文件
    cfg = fopen(filename, module);
    if (!cfg)
      return -1;
  }
  return 0;
}


// 读函数变量值字符串
const char* get_cfg_string(const char* funname, const char* var, char* val, int v_l, const char* defval)
{
  do
  {
    // 打配置文件
    if (open_exe_cfg(READ_MODULE_F) < 0)
      break;

    // 读取函数变量值
    const char* value = get_value(funname, var, v_l, val);
    if (!value)
      break;

    return value;
  } while(0);
  if (defval)
    snprintf(val, v_l, "%s", defval);
  return defval;
}

// 读函数整型变量值
long get_cfg_int(const char* funname, const char* var, long defval)
{
  do
  {
    if (open_exe_cfg(READ_MODULE_F) < 0)
      break;

    char val[256];
    const char* value = get_value(funname, var, 256, val);

    if (!value)
      break;

    defval = atol(value);
  } while(0);
  return defval;
}

// 写函数名
int write_func(const char* funname)
{
  if (!cfg)
    return -1;
  return fprintf(cfg, "%s(){\n", funname);
}

// 写函数字符串变量值
int write_string(const char* key, const char* value)
{
  if (!cfg)
    return -1;
  return fprintf(cfg, "\t%s = %s\n", key, value);
}

// 写函数整型变量值
int write_int(const char* key, long value)
{
  if (!cfg)
    return -1;
  return fprintf(cfg, "\t%s = %lu\n", key, value);
}

// 写函数结尾
int write_func_end()
{
  if (!cfg)
    return -1;
  return fprintf(cfg, "}\n");
}

// 关闭配置文件
void close_cfg()
{
  if (cfg)
    fclose(cfg);
  cfg = 0;
}
