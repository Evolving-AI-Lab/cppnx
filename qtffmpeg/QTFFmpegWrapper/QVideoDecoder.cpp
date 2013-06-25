/*
	QTFFmpegWrapper - QT FFmpeg Wrapper Class 
	Copyright (C) 2009,2010:
			Daniel Roggen, droggen@gmail.com			
			
	All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "QVideoDecoder.h"
#include <limits.h>
#include <stdint.h>
#include "ffmpeg.h"


/******************************************************************************
*******************************************************************************
* QVideoDecoder   QVideoDecoder   QVideoDecoder   QVideoDecoder   QVideoDecoder
*******************************************************************************
******************************************************************************/

/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/


/**
   \brief Constructor - opens a video on later openFile call
**/
QVideoDecoder::QVideoDecoder()
{
   InitVars();
   initCodec();
}
/**
   \brief Constructor - opens directly a video
**/
QVideoDecoder::QVideoDecoder(QString file)
{
   InitVars();
   initCodec();

   ok = openFile(file.toStdString().c_str());
}

QVideoDecoder::~QVideoDecoder()
{
   close();
}

void QVideoDecoder::InitVars()
{
   ok=false;
   pFormatCtx=0;
   pCodecCtx=0;
   pCodec=0;
   pFrame=0;
   pFrameRGB=0;
   buffer=0;
   img_convert_ctx=0;
}

void QVideoDecoder::close()
{
   if(!ok)
      return;

   // Free the RGB image
   if(buffer)
      delete [] buffer;

   // Free the YUV frame
   if(pFrame)
      av_free(pFrame);

   // Free the RGB frame
   if(pFrameRGB)
      av_free(pFrameRGB);

   // Close the codec
   if(pCodecCtx)
      avcodec_close(pCodecCtx);

   // Close the video file
   if(pFormatCtx)
      av_close_input_file(pFormatCtx);

   InitVars();
}


bool QVideoDecoder::initCodec()
{
   ffmpeg::avcodec_init();
   ffmpeg::avcodec_register_all();
   ffmpeg::av_register_all();

   printf("License: %s\n",ffmpeg::avformat_license());
   printf("AVCodec version %d\n",ffmpeg::avformat_version());
   printf("AVFormat configuration: %s\n",ffmpeg::avformat_configuration());

   return true;
}

bool QVideoDecoder::openFile(QString filename)
{
   // Close last video..
   close();

   LastLastFrameTime=INT_MIN;       // Last last must be small to handle the seek well
   LastFrameTime=0;
   LastLastFrameNumber=INT_MIN;
   LastFrameNumber=0;
   DesiredFrameTime=DesiredFrameNumber=0;
   LastFrameOk=false;


   // Open video file
   if(av_open_input_file(&pFormatCtx, filename.toStdString().c_str(), NULL, 0, NULL)!=0)
       return false; // Couldn't open file

   // Retrieve stream information
   if(av_find_stream_info(pFormatCtx)<0)
       return false; // Couldn't find stream information

   // Dump information about file onto standard error
   dump_format(pFormatCtx, 0, filename.toStdString().c_str(), false);

   // Find the first video stream
   videoStream=-1;
   for(unsigned i=0; i<pFormatCtx->nb_streams; i++)
       if(pFormatCtx->streams[i]->codec->codec_type==ffmpeg::AVMEDIA_TYPE_VIDEO)
       {
           videoStream=i;
           break;
       }
   if(videoStream==-1)
       return false; // Didn't find a video stream

   // Get a pointer to the codec context for the video stream
   pCodecCtx=pFormatCtx->streams[videoStream]->codec;

   // Find the decoder for the video stream
   pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
   if(pCodec==NULL)
       return false; // Codec not found

   // Open codec
   if(avcodec_open(pCodecCtx, pCodec)<0)
       return false; // Could not open codec

   // Hack to correct wrong frame rates that seem to be generated by some
   // codecs
  if(pCodecCtx->time_base.num>1000 && pCodecCtx->time_base.den==1)
     pCodecCtx->time_base.den=1000;

   // Allocate video frame
   pFrame=ffmpeg::avcodec_alloc_frame();

   // Allocate an AVFrame structure
   pFrameRGB=ffmpeg::avcodec_alloc_frame();
   if(pFrameRGB==NULL)
       return false;

   // Determine required buffer size and allocate buffer
   numBytes=ffmpeg::avpicture_get_size(ffmpeg::PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
   buffer=new uint8_t[numBytes];

   // Assign appropriate parts of buffer to image planes in pFrameRGB
   avpicture_fill((ffmpeg::AVPicture *)pFrameRGB, buffer, ffmpeg::PIX_FMT_RGB24,
       pCodecCtx->width, pCodecCtx->height);

   ok=true;
   return true;
}
bool QVideoDecoder::isOk()
{
   return ok;
}

/**
   Decodes the video stream until the first frame with number larger or equal than 'after' is found.

   Returns:
   - true if a frame is found, false otherwise.
   - the image as a QImage if img is non-null
   - time frame time, if frametime is non-null
   - the frame number, if framenumber is non-null

   All times are in milliseconds.
**/
bool QVideoDecoder::decodeSeekFrame(int after)
{
   if(!ok)
      return false;

   //printf("decodeSeekFrame. after: %d. LLT: %d. LT: %d. LLF: %d. LF: %d. LastFrameOk: %d.\n",after,LastLastFrameTime,LastFrameTime,LastLastFrameNumber,LastFrameNumber,(int)LastFrameOk);



   // If the last decoded frame satisfies the time condition we return it
   //if( after!=-1 && ( LastDataInvalid==false && after>=LastLastFrameTime && after <= LastFrameTime))
   if( after!=-1 && ( LastFrameOk==true && after>=LastLastFrameNumber && after <= LastFrameNumber))
   {
      // This is the frame we want to return

      // Compute desired frame time
      ffmpeg::AVRational millisecondbase = {1, 1000};
      DesiredFrameTime = ffmpeg::av_rescale_q(after,pFormatCtx->streams[videoStream]->time_base,millisecondbase);

      //printf("Returning already available frame %d @ %d. DesiredFrameTime: %d\n",LastFrameNumber,LastFrameTime,DesiredFrameTime);

      return true;
   }   

   // The last decoded frame wasn't ok; either we need any new frame (after=-1), or a specific new frame with time>after

   bool done=false;
   while(!done)
   {
      // Read a frame
      if(av_read_frame(pFormatCtx, &packet)<0)
         return false;                             // Frame read failed (e.g. end of stream)

      //printf("Packet of stream %d, size %d\n",packet.stream_index,packet.size);

      if(packet.stream_index==videoStream)
      {
         // Is this a packet from the video stream -> decode video frame

         int frameFinished;
         avcodec_decode_video2(pCodecCtx,pFrame,&frameFinished,&packet);

         //printf("used %d out of %d bytes\n",len,packet.size);

         //printf("Frame type: ");
         //if(pFrame->pict_type == FF_B_TYPE)
         //   printf("B\n");
         //else if (pFrame->pict_type == FF_I_TYPE)
         //   printf("I\n");
         //else
         //   printf("P\n");


         /*printf("codecctx time base: num: %d den: %d\n",pCodecCtx->time_base.num,pCodecCtx->time_base.den);
         printf("formatctx time base: num: %d den: %d\n",pFormatCtx->streams[videoStream]->time_base.num,pFormatCtx->streams[videoStream]->time_base.den);
         printf("pts: %ld\n",pts);
         printf("dts: %ld\n",dts);*/




         // Did we get a video frame?
         if(frameFinished)
         {
            ffmpeg::AVRational millisecondbase = {1, 1000};
            int f = packet.dts;
            int t = ffmpeg::av_rescale_q(packet.dts,pFormatCtx->streams[videoStream]->time_base,millisecondbase);
            if(LastFrameOk==false)
            {
               LastFrameOk=true;
               LastLastFrameTime=LastFrameTime=t;
               LastLastFrameNumber=LastFrameNumber=f;
            }
            else
            {
               // If we decoded 2 frames in a row, the last times are okay
               LastLastFrameTime = LastFrameTime;
               LastLastFrameNumber = LastFrameNumber;
               LastFrameTime=t;
               LastFrameNumber=f;
            }
            //printf("Frame %d @ %d. LastLastT: %d. LastLastF: %d. LastFrameOk: %d\n",LastFrameNumber,LastFrameTime,LastLastFrameTime,LastLastFrameNumber,(int)LastFrameOk);

            // Is this frame the desired frame?
            if(after==-1 || LastFrameNumber>=after)
            {
               // It's the desired frame

               // Convert the image format (init the context the first time)
               int w = pCodecCtx->width;
               int h = pCodecCtx->height;
               img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx,w, h, pCodecCtx->pix_fmt, w, h, ffmpeg::PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

               if(img_convert_ctx == NULL)
               {
                  printf("Cannot initialize the conversion context!\n");
                  return false;
               }
               ffmpeg::sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

               // Convert the frame to QImage
               LastFrame=QImage(w,h,QImage::Format_RGB888);

               for(int y=0;y<h;y++)
                  memcpy(LastFrame.scanLine(y),pFrameRGB->data[0]+y*pFrameRGB->linesize[0],w*3);

               // Set the time
               DesiredFrameTime = ffmpeg::av_rescale_q(after,pFormatCtx->streams[videoStream]->time_base,millisecondbase);
               LastFrameOk=true;


               done = true;

            } // frame of interest
         }  // frameFinished
      }  // stream_index==videoStream
      av_free_packet(&packet);      // Free the packet that was allocated by av_read_frame
   }
   //printf("Returning new frame %d @ %d. LastLastT: %d. LastLastF: %d. LastFrameOk: %d\n",LastFrameNumber,LastFrameTime,LastLastFrameTime,LastLastFrameNumber,(int)LastFrameOk);
   //printf("\n");
   return done;   // done indicates whether or not we found a frame
}

/**
   \brief Decodes the next frame in the video stream


**/
bool QVideoDecoder::seekNextFrame()
{
   bool ret = decodeSeekFrame(DesiredFrameNumber+1);

   if(ret)
      DesiredFrameNumber++;   // Only updates the DesiredFrameNumber if we were successful in getting that frame
   else
      LastFrameOk=false;      // We didn't find the next frame (e.g. seek out of range) - mark we don't know where we are.
   return ret;
}

/**
  \brief Seek to millisecond
**/
bool QVideoDecoder::seekMs(int tsms)
{
   if(!ok)
      return false;


   //printf("**** SEEK TO ms %d. LLT: %d. LT: %d. LLF: %d. LF: %d. LastFrameOk: %d\n",tsms,LastLastFrameTime,LastFrameTime,LastLastFrameNumber,LastFrameNumber,(int)LastFrameOk);

   // Convert time into frame number
   DesiredFrameNumber = ffmpeg::av_rescale(tsms,pFormatCtx->streams[videoStream]->time_base.den,pFormatCtx->streams[videoStream]->time_base.num);
   DesiredFrameNumber/=1000;

   return seekFrame(DesiredFrameNumber);
}
/**
  \brief Seek to frame
**/
bool QVideoDecoder::seekFrame(int64_t frame)
{

   if(!ok)
      return false;

   //printf("**** seekFrame to %d. LLT: %d. LT: %d. LLF: %d. LF: %d. LastFrameOk: %d\n",(int)frame,LastLastFrameTime,LastFrameTime,LastLastFrameNumber,LastFrameNumber,(int)LastFrameOk);

   // Seek if:
   // - we don't know where we are (Ok=false)
   // - we know where we are but:
   //    - the desired frame is after the last decoded frame (this could be optimized: if the distance is small, calling decodeSeekFrame may be faster than seeking from the last key frame)
   //    - the desired frame is smaller or equal than the previous to the last decoded frame. Equal because if frame==LastLastFrameNumber we don't want the LastFrame, but the one before->we need to seek there
   if( (LastFrameOk==false) || ((LastFrameOk==true) && (frame<=LastLastFrameNumber || frame>LastFrameNumber) ) )
   {
      //printf("\t avformat_seek_file\n");
      if(ffmpeg::avformat_seek_file(pFormatCtx,videoStream,0,frame,frame,AVSEEK_FLAG_FRAME)<0)
         return false;

      avcodec_flush_buffers(pCodecCtx);

      DesiredFrameNumber = frame;
      LastFrameOk=false;
   }
   //printf("\t decodeSeekFrame\n");

   return decodeSeekFrame(frame);

   return true;
}



bool QVideoDecoder::getFrame(QImage&img,int *effectiveframenumber,int *effectiveframetime,int *desiredframenumber,int *desiredframetime)
{
   img = LastFrame;

   if(effectiveframenumber)
      *effectiveframenumber = LastFrameNumber;
   if(effectiveframetime)
      *effectiveframetime = LastFrameTime;
   if(desiredframenumber)
      *desiredframenumber = DesiredFrameNumber;
   if(desiredframetime)
      *desiredframetime = DesiredFrameTime;

   //printf("getFrame. Returning valid? %s. Desired %d @ %d. Effective %d @ %d\n",LastFrameOk?"yes":"no",DesiredFrameNumber,DesiredFrameTime,LastFrameNumber,LastFrameTime);

   return LastFrameOk;
}


/**
  \brief Debug function: saves a frame as PPM
**/
void QVideoDecoder::saveFramePPM(ffmpeg::AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}



void QVideoDecoder::dumpFormat(ffmpeg::AVFormatContext *ic,
                 int index,
                 const char *url,
                 int is_output)
{
    //int i;
    uint8_t *printed = (uint8_t*)ffmpeg::av_mallocz(ic->nb_streams);
    if (ic->nb_streams && !printed)
        return;

    printf("AV_TIME_BASE: %d\n",AV_TIME_BASE);

    printf("%s #%d, %s,\n %s '%s':\n",
            is_output ? "Output" : "Input",
            index,
            is_output ? ic->oformat->name : ic->iformat->name,
            is_output ? "to" : "from", url);
    if (!is_output) {
        printf("  Duration: ");
        //if (ic->duration != AV_NOPTS_VALUE)
        {
            int hours, mins, secs, us;
            secs = ic->duration / AV_TIME_BASE;
            us = ic->duration % AV_TIME_BASE;
            mins = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            printf("%02d:%02d:%02d.%02d\n", hours, mins, secs,
                   (100 * us) / AV_TIME_BASE);
        } //else {
            //printf("N/A");
        //}
        //if (ic->start_time != AV_NOPTS_VALUE)
        {
            int secs, us;
            printf(", start: ");
            secs = ic->start_time / AV_TIME_BASE;
            us = ic->start_time % AV_TIME_BASE;
            printf("%d.%06d\n",
                   secs, (int)ffmpeg::av_rescale(us, 1000000, AV_TIME_BASE));
        }
        printf(", bitrate: ");
        if (ic->bit_rate) {
            printf("%d kb/s\n", ic->bit_rate / 1000);
        } else {
            printf("N/A\n");
        }
        printf("\n");
    }
    if(ic->nb_programs) {
        unsigned int j, total=0;
        for(j=0; j<ic->nb_programs; j++) {
            ffmpeg::AVMetadataTag *name = av_metadata_get(ic->programs[j]->metadata,
                                                  "name", NULL, 0);
            printf("  Program %d %s\n", ic->programs[j]->id,
                   name ? name->value : "");
            /*for(k=0; k<ic->programs[j]->nb_stream_indexes; k++) {
                dump_stream_format(ic, ic->programs[j]->stream_index[k], index, is_output);
                printed[ic->programs[j]->stream_index[k]] = 1;
            }*/
            total += ic->programs[j]->nb_stream_indexes;
        }
        if (total < ic->nb_streams)
            printf( "  No Program\n");
    }
    /*for(i=0;i<ic->nb_streams;i++)
        if (!printed[i])
            ffmpeg::dump_stream_format(ic, i, index, is_output);*/

    if (ic->metadata) {
        ffmpeg::AVMetadataTag *tag=NULL;
        printf("  Metadata\n");
        while((tag=av_metadata_get(ic->metadata, "", tag, AV_METADATA_IGNORE_SUFFIX))) {
            printf("    %-16s: %s\n", tag->key, tag->value);
        }
    }
    ffmpeg::av_free(printed);
}

int QVideoDecoder::getVideoLengthMs()
{
   if(!isOk())
      return -1;


   int secs = pFormatCtx->duration / AV_TIME_BASE;
   int us = pFormatCtx->duration % AV_TIME_BASE;
   int l = secs*1000 + us/1000;


   dumpFormat(pFormatCtx,videoStream,"test video",0);

   return l;
}
