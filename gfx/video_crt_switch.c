/* CRT SwitchRes Core 
 * Copyright (C) 2018 Alphanu / Ben Templeman.
 *
 * RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2017 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "video_driver.h"
#include "video_crt_switch.h"
#include "video_display_server.h"

#if defined(__arm__)
   #include "userland/interface/vmcs_host/vc_vchi_gencmd.h"
#endif

static unsigned ra_core_width     = 0;
static unsigned ra_core_height    = 0;
static unsigned ra_tmp_width      = 0;
static unsigned ra_tmp_height     = 0;
static unsigned ra_set_core_hz    = 0;
static unsigned orig_width        = 0;
static unsigned orig_height       = 0;

static bool first_run             = true;

static float ra_tmp_core_hz       = 0.0f;
static float fly_aspect           = 0.0f;
static float ra_core_hz           = 0.0f;

static void crt_check_first_run(void)
{		
   if (!first_run)
      return;

   first_run   = false;
}

static void switch_crt_hz(void)
{
   if (ra_core_hz == ra_tmp_core_hz)
      return;
   /* set hz float to an int for windows switching */
   if (ra_core_hz < 53)
      ra_set_core_hz = 50;	
   if (ra_core_hz >= 53  &&  ra_core_hz < 57)
      ra_set_core_hz = 55;	
   if (ra_core_hz >= 57)
      ra_set_core_hz = 60;	
	
   video_monitor_set_refresh_rate(ra_set_core_hz);
   
   ra_tmp_core_hz = ra_core_hz;
}


void crt_aspect_ratio_switch(unsigned width, unsigned height)
{
   /* send aspect float to videeo_driver */
   fly_aspect = (float)width / height;
   video_driver_set_aspect_ratio_value((float)fly_aspect);
}

static void switch_res_crt(unsigned width, unsigned height)
{
 //if (height > 100)
 //  {
      video_display_server_switch_resolution(width, height,
            ra_set_core_hz, ra_core_hz);
      #if defined(__arm__)
      crt_rpi_switch(width, height, ra_set_core_hz);
      #endif
      crt_switch_driver_reinit();
	 
	  
 // }
}

/* Create correct aspect to fit video if resolution does not exist */
void crt_screen_setup_aspect(unsigned width, unsigned height)
{
   
   switch_crt_hz();
   /* get original resolution of core */	
   if (height == 4)
   {
      /* detect menu only */	
      if (width < 1920)
         width = 320;
      
      height = 240;
      crt_aspect_ratio_switch(width, height);
   }

   if (height < 191 && height != 144)
   {				
      crt_aspect_ratio_switch(width, height);
      height = 200;
   }	

   if (height > 191)
      crt_aspect_ratio_switch(width, height);

   if (height == 144 && ra_set_core_hz == 50)
   {				
      height = 288;
      crt_aspect_ratio_switch(width, height);
   }

   if (height > 200 && height < 224)
   {				
      crt_aspect_ratio_switch(width, height);
      height = 224;
   }

   if (height > 224 && height < 240)
   {				
      crt_aspect_ratio_switch(width, height);
      height = 240;
   }

   if (height > 240 && height < 255)
   {								
      crt_aspect_ratio_switch(width, height);
      height = 254;
   }

   if (height == 528 && ra_set_core_hz == 60)
   {								
      crt_aspect_ratio_switch(width, height);
      height = 480;
   }

   if (height >= 240 && height < 255 && ra_set_core_hz == 55)
   {
      crt_aspect_ratio_switch(width, height);
      height = 254;
   }

 }


void crt_switch_res_core(unsigned width, unsigned height, float hz)
{
   /* ra_core_hz float passed from within 
    * void video_driver_monitor_adjust_system_rates(void) */
   ra_core_width  = width;		
   ra_core_height = height;
   ra_core_hz     = hz;

   crt_check_first_run();

   /* Detect resolution change and switch */
	  if (
         (ra_tmp_height != ra_core_height) || 
         (ra_core_width != ra_tmp_width)
      )
	  {
    crt_screen_setup_aspect(width, height);
	switch_res_crt(width, height);

	}
   ra_tmp_height  = ra_core_height;
   ra_tmp_width   = ra_core_width;

   /* Check if aspect is correct, if notchange */
       
}

void crt_video_restore(void)
{
   if (first_run)
      return;

    first_run = true;
}


// void crt_rpi_switch(void)
// {
// static char output[250]         = {0};   
 //  static char output1[250]         = {0}; 
  // static char output2[250]         = {0}; 
  // 
  // if (fork() == 0) {

	//sprintf(output,"bash -c \"vcgencmd hdmi_timings 1920 1 106 169 480 240 1 1 3 5 0 0 0 60 0 41458500 1 \" ");
 // system(output);
//VCHI_INSTANCE_T vchi_instance;
//VCHI_CONNECTION_T *vchi_connection = NULL;
//char buffer[1024];

//vcos_init ();

//vchi_initialise (&vchi_instance);
       // fatal ("VCHI initialization failed");

//create a vchi connection
//vchi_connect (NULL, 0, vchi_instance);
       // fatal ("VCHI connection failed");

//vc_vchi_gencmd_init (vchi_instance, &vchi_connection, 1);


//vc_gencmd (buffer, sizeof (buffer), set_hdmi_timing);
	//fatal ("Failed to set non-interpolation scaling kernel");

    //vc_gencmd_stop ();

    //close the vchi connection
  // vchi_disconnect (vchi_instance);
       // fatal ("VCHI disconnect failed");
      // vc_vchi_gencmd_deinit();
//vcos_deinit();
//  exit(0);

   
  //   }  
    
//   sprintf(output1,"tvservice -e \"DMT 87\" > /dev/null");
 //  system(output1);
 //  sprintf(output2,"fbset -g 1920 240 1920 240 24 > /dev/null");
 //  system(output2);/
//}
float get_fly_aspect(void)
{
	return fly_aspect;
}


#if defined(__arm__)
void crt_rpi_switch(int width, int height, int hz)
{
   static char output[250]         = {0};   
   static char output1[250]         = {0}; 
   static char output2[250]         = {0}; 
   static char set_hdmi[250]       ={0};
   static char set_hdmi_timing[250]    = {0};
   int i              = 0;
   int hfp            = 0;
   int hsp            = 0;
   int hbp            = 0;
   int vfp            = 0;
   int vsp            = 0;
   int vbp            = 0;
   int hmax           = 0;
   int vmax           = 0;
   int pdefault       = 8;
   int pwidth         = 0;
   float roundw     = 0.0f;
   float roundh     = 0.0f;
   float pixel_clock  = 0;

   //crt_en = true;

   /* set core refresh from hz */
   video_monitor_set_refresh_rate(hz);

   /* following code is the mode line generator */

   pwidth = width;

   if (height < 400 && width > 400)
      pwidth = width / 2;

   roundw = roundf((float)pwidth / (float)height * 100) / 100;

   if (height > width)
      roundw = roundf((float)height / (float)width * 100) / 100;

   if (roundw > 1.35)
      roundw = 1.25;

   if (roundw < 1.20)
      roundw = 1.34;
   //hfp = width * 0.055;
   hfp=106;
   //hsp = width * 0.140-hfp;
   hsp = 169;
 //  hbp = (width * roundw - 8)-width-hfp;
   hbp = 480;
  // hmax = width - hbp - hsp;

   if (height < 241)
      vmax = 261;
   if (height < 241 && hz > 56 && hz < 58)
      vmax = 280;
   if (height < 241 && hz < 55)
      vmax = 313;
   if (height > 250 && height < 260 && hz > 54)
      vmax = 296;
   if (height > 250 && height < 260 && hz > 52 && hz < 54)
      vmax = 285;
   if (height > 250 && height < 260 && hz < 52)
      vmax = 313;
   if (height > 260 && height < 300)
      vmax = 318;

   if (height > 400 && hz > 56)
      vmax = 533;
   if (height > 520 && hz < 57)
      vmax = 580;

   if (height > 300 && hz < 56)
      vmax = 615;
   if (height > 500 && hz < 56)
      vmax = 624;
   if (height > 300)
      pdefault = pdefault * 2;

   //vfp = (height + ((vmax - height) / 2) - pdefault) - height;
	vfp = 1;
   //if (height < 300)
     // vsp = vfp + 3; /* needs to be 3 for progressive */
   //if (height > 300)
   //   vsp = vfp + 6; /* needs to be 6 for interlaced */

	vsp = 3;

  // vbp = (vmax-height)-vsp-vfp;
  vbp = 5;
  hmax = width+hfp+hsp+hbp;
  vmax = 249;

   if (height < 300)
      pixel_clock = (hmax * vmax * hz) ;

   if (height > 300)
      pixel_clock = ((hmax * vmax * hz) ) / 2;
   /* above code is the modeline generator */
   
    pixel_clock = 41458500;
   
   // if (fork() == 0) {
      sprintf(set_hdmi, "hdmi_timings 1920 1 106 169 480 240 1 1 3 5 0 0 0 60 0 41458500 %d ", 1);
	  sprintf(set_hdmi_timings, "hdmi_timings %d 1 %d %d %d %d 1 %d %d %d 0 0 0 %d 0 %lf 1 ", width, hfp, hsp, hbp, height, vfp, vsp, vbp, hz, pixel_clock); 
   //HRES, HSYNCPOLARITY, HFRONTPORCH, HSYNCPORCH, HBACKPORCH, VRES, VSYNCPOLARITY, VFRONTPORCH, VSYNCPULSE, VBACKPORCH, 0, 0, 0, HZ, PROG/INTERLACED, DOTCLOCK, 1
	 //  set_hdmi_timing[] = set_hdmi;
      VCHI_INSTANCE_T vchi_instance;
      VCHI_CONNECTION_T *vchi_connection = NULL;
      char buffer[1024];

      vcos_init ();

      vchi_initialise (&vchi_instance);
            // fatal ("VCHI initialization failed");

      //create a vchi connection
      vchi_connect (NULL, 0, vchi_instance);
            // fatal ("VCHI connection failed");

      vc_vchi_gencmd_init (vchi_instance, &vchi_connection, 1);


      vc_gencmd (buffer, sizeof (buffer), set_hdmi);
         //fatal ("Failed to set non-interpolation scaling kernel");

      vc_gencmd_stop ();

         //close the vchi connection
      vchi_disconnect (vchi_instance);
            // fatal ("VCHI disconnect failed");

    //   exit(0);

   
       
   // }
   sprintf(output1,"tvservice -e \"DMT 87\" > /dev/null");
   system(output1);
   sprintf(output2,"fbset -g 1920 240 1920 240 24 > /dev/null");
   system(output2);
}
#endif


