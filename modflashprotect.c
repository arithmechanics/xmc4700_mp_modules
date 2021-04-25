//This is FLASH PROTECT example from XMCLIB, 
//converted into a Micropython module.
//
//This is a DRAFT created by Dmitriy L. for DEMO purposes.
//

#include <stdlib.h>     /* strtol */
#include "py/runtime.h"
//#include <xmc_common.h>
#include <xmc_scu.h>
#include <xmc_flash.h>

#define shell_println(S) mp_printf(&mp_plat_print, (S "\n"))

const bool parsePassword(mp_obj_t arg, uint32_t* p0, uint32_t* p1) {
  const char* s_type = mp_obj_get_type_str(arg);
  if(strcmp(s_type, "str") !=0)  return false;
  const char* s = mp_obj_str_get_str(arg);
  if(strlen(s) != 8) {
	  mp_printf(&mp_plat_print, "parsePassword: got %s", s);
          return false;
  }

  uint8_t* p0bytes = (uint8_t*)p0;
  uint8_t* p1bytes = (uint8_t*)p1;
  for(int i=0; i<4; ++i) {
	  p0bytes[i] = s[i];
	  p1bytes[i] = s[i+4];
  }
  return true;
}

//static void flash_protection_install_cmd(int32_t argc, char **argv)
STATIC mp_obj_t flashprotect_install(mp_obj_t arg) 
{
  //XMC_UNUSED_ARG(argc);
  uint32_t p0, p1;
  if(!parsePassword(arg, &p0, &p1)) {
	  shell_println("Password should be a string of 8 letters");
	  return mp_const_none;
  }
  mp_printf(&mp_plat_print, "flashprotect_install uses p0=%d, p1=%d\n", p0, p1);

  if ((XMC_FLASH_GetStatus() & XMC_FLASH_STATUS_PROTECTION_INSTALLED) == 0)
  {
    /* if no protection is installed */
    XMC_FLASH_EraseUCB(XMC_FLASH_UCB0);
    XMC_FLASH_InstallProtection(0, XMC_FLASH_PROTECTION_READ_GLOBAL, p0, p1);
    XMC_FLASH_ConfirmProtection(0);
		
    shell_println("Resetting device...");
    XMC_SCU_RESET_ClearDeviceResetReason();
    NVIC_SystemReset();
  }	
  else
  {
	shell_println("ERROR: Protection already installed!!");
  }
  return mp_const_none;
}

//static void flash_protection_uninstall_cmd(int32_t argc, char **argv)
STATIC mp_obj_t flashprotect_uninstall(mp_obj_t arg) 
{
  //XMC_UNUSED_ARG(argc);
  uint32_t p0, p1;
  if(!parsePassword(arg, &p0, &p1)) {
          shell_println("Password should be a string of 8 letters");
          return mp_const_none;
  }
  mp_printf(&mp_plat_print, "flashprotect_uninstall uses p0=%d, p1=%d\n", p0, p1);

  if ((XMC_FLASH_GetStatus() & XMC_FLASH_STATUS_PROTECTION_INSTALLED) != 0)	
  {
    if (XMC_FLASH_VerifyReadProtection(p0, p1))
	{
      shell_println("Protection disabled");
      
  	  shell_println("Erase UCB");
      XMC_FLASH_EraseUCB(XMC_FLASH_UCB0);
      
	  shell_println("Resetting device...");
      XMC_SCU_RESET_ClearDeviceResetReason();
      NVIC_SystemReset();
    }
  }

  return mp_const_none;
}

//static void flash_protection_disable_cmd(int32_t argc, char **argv)
STATIC mp_obj_t flashprotect_pause(mp_obj_t arg) 
{
  //XMC_UNUSED_ARG(argc);
  uint32_t p0, p1;
  if(!parsePassword(arg, &p0, &p1)) {
          shell_println("Password should be a string of 8 letters");
          return mp_const_none;
  }

  if ((XMC_FLASH_GetStatus() & XMC_FLASH_STATUS_PROTECTION_INSTALLED) != 0)	
  {
    if (XMC_FLASH_VerifyReadProtection(p0, p1))
    {
      shell_println("Protection disabled");
    }
    else
    {
      shell_println("ERROR: A problem occur while disabling the read protection!!");
 	  }
  }
  else
  {
    shell_println("ERROR: Protection not yet installed!!");		
  }

  return mp_const_none;
}

//static void flash_protection_resume_cmd(int32_t argc, char **argv)
STATIC mp_obj_t flashprotect_resume(void) 
{
  //XMC_UNUSED_ARG(argc);
  //XMC_UNUSED_ARG(argv);

  XMC_FLASH_ResumeProtection();
  shell_println("Protection re-enabled");
  return mp_const_none;
}

//static void flash_protection_status_cmd(int32_t argc, char **argv)
STATIC mp_obj_t flashprotect_info(void) 
{
  //XMC_UNUSED_ARG(argc);
  //XMC_UNUSED_ARG(argv);

  uint32_t status = XMC_FLASH_GetStatus();
  if ((status & XMC_FLASH_STATUS_PROTECTION_INSTALLED) != 0)
  {
    shell_println("Protection installed");
    if ((status & XMC_FLASH_STATUS_READ_PROTECTION_DISABLED_STATE) != 0)
    {
      shell_println("Read protection temporarily disabled");
	}
  }
  else
  {
	shell_println("Protection not installed");
  }

  return mp_const_none;
}


STATIC MP_DEFINE_CONST_FUN_OBJ_0(flashprotect_info_obj, flashprotect_info);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(flashprotect_install_obj, flashprotect_install);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(flashprotect_uninstall_obj, flashprotect_uninstall);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(flashprotect_pause_obj, flashprotect_pause);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(flashprotect_resume_obj, flashprotect_resume);


STATIC const mp_rom_map_elem_t flashprotect_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_flashprotect) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&flashprotect_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_install), MP_ROM_PTR(&flashprotect_install_obj) },
    { MP_ROM_QSTR(MP_QSTR_uninstall), MP_ROM_PTR(&flashprotect_uninstall_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&flashprotect_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&flashprotect_resume_obj) },
};

STATIC MP_DEFINE_CONST_DICT(flashprotect_module_globals, flashprotect_module_globals_table);

const struct _mp_obj_module_t mp_module_flashprotect = {
    .base = { &mp_type_module },
    .name = MP_QSTR_flashprotect,
    .globals = (mp_obj_dict_t *)&flashprotect_module_globals,
};


