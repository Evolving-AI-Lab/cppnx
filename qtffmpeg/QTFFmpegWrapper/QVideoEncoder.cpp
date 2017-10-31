/*
   QTFFmpegWrapper - QT FFmpeg Wrapper Class
   Copyright (C) 2009-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QPainter>
#include <iostream>
#include "QVideoEncoder.h"
#include "ffmpeg.h"



/******************************************************************************
*******************************************************************************
* QVideoEncoder   QVideoEncoder   QVideoEncoder   QVideoEncoder   QVideoEncoder
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/

/**
  gop: maximal interval in frames between keyframes
**/
QVideoEncoder::QVideoEncoder()
{
   initVars();
   initCodec();
}

QVideoEncoder::~QVideoEncoder()
{
   close();
   /*if(Initmodefile)
   {
      writeFooter();
      Outdev->close();
      delete Outdev;
      Outdev=0;
   }
   else
   {
      // nothing to do
   }*/
}

bool QVideoEncoder::createFile(QString fileName,unsigned width,unsigned height,unsigned bitrate,unsigned gop,unsigned fps)
{
   // If we had an open video, close it.
   close();

   // Set basic parameters
   Width=width;
   Height=height;
   Gop=gop;
   Bitrate=bitrate;

   // Determine output format based on file name
   pOutputFormat = ffmpeg::av_guess_format(NULL, fileName.toStdString().c_str(), NULL);
   if (!pOutputFormat) {
      printf("Could not deduce output format from file extension: using MPEG.\n");
      pOutputFormat = ffmpeg::av_guess_format("mpeg", NULL, NULL);
   }

   // Initialize the format context
   pFormatCtx=ffmpeg::avformat_alloc_context();
   if(!pFormatCtx)
   {
      printf("Error allocating format context\n");
      return false;
   }
   pFormatCtx->oformat = pOutputFormat;
   snprintf(pFormatCtx->filename, sizeof(pFormatCtx->filename), "%s", fileName.toStdString().c_str());

   // Find the video encoder
   ffmpeg::AVCodecID codec_id = ffmpeg::av_guess_codec(pOutputFormat,
													   NULL,
													   NULL,
													   NULL,
													   ffmpeg::AVMEDIA_TYPE_VIDEO);
   pCodec = avcodec_find_encoder(codec_id);
   if (!pCodec)
   {
	   printf("codec not found for: %i\n", codec_id);
	   return false;
   }

   // Initalize the video stream
   pVideoStream = avformat_new_stream(pFormatCtx, pCodec);
   if(!pVideoStream )
   {
      printf("Could not allocate stream\n");
      return false;
   }
   
   // Initialize codec context
   pCodecCtx=avcodec_alloc_context3(pCodec);
   pCodecCtx->codec_id = codec_id;
   pCodecCtx->codec_type = ffmpeg::AVMEDIA_TYPE_VIDEO;
   pCodecCtx->bit_rate = Bitrate;
   pCodecCtx->width = getWidth();
   pCodecCtx->height = getHeight();
   pCodecCtx->time_base.den = fps;
   pCodecCtx->time_base.num = 1;
   pCodecCtx->gop_size = Gop;
   pCodecCtx->pix_fmt = ffmpeg::AV_PIX_FMT_YUV420P;
   //pCodecCtx->rc_buffer_size = 36000000 35157*1024;
   pCodecCtx->rc_buffer_size = 35157*1024;
   //pCodecCtx->rc_initial_buffer_occupancy = 36000000;
   pCodecCtx->rc_initial_buffer_occupancy = 35157*1024;
   pCodecCtx->rc_max_rate = Bitrate*8;
   pCodecCtx->qcompress = 1;
   pCodecCtx->qmin = 1;
   pCodecCtx->qmax = 4;

   // some formats want stream headers to be separate
   if(pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
      pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

   // Set the codec parameters for the video stream
   avcodec_parameters_from_context(pVideoStream->codecpar, pCodecCtx);
   ffmpeg::AVCPBProperties *props;
   props = (ffmpeg::AVCPBProperties*) ffmpeg::av_stream_new_side_data(
       pVideoStream, ffmpeg::AV_PKT_DATA_CPB_PROPERTIES, sizeof(*props));
   //props->buffer_size = 230 * 1024;
   //props->buffer_size = 460 * 1024;
   //props->buffer_size = 920 * 1024;
   //props->buffer_size = 1840 * 1024;
   //props->buffer_size = 3680 * 1024;
   props->buffer_size = 7360 * 1024;
   //props->buffer_size = 35157*1024;
   //props->max_bitrate = Bitrate*256;
   //props->max_bitrate = Bitrate*128;
   //props->max_bitrate = Bitrate*64;
   props->max_bitrate = Bitrate*32;
   //props->max_bitrate = Bitrate*16;
   //props->max_bitrate = Bitrate*8;
   //props->max_bitrate = Bitrate*4;
   //props->max_bitrate = Bitrate*2;
   props->min_bitrate = 0;
   props->avg_bitrate = Bitrate;
   props->vbv_delay = UINT64_MAX;
    
   // props = (ffmpeg::AVCPBProperties*)av_stream_get_side_data(pVideoStream, ffmpeg::AV_PKT_DATA_CPB_PROPERTIES, NULL);
   // if(props==NULL){
   //    printf("Stream did not have required side properties.\n");
   //    return false;
   // }
   //props->buffer_size = 230 * 1024;

//   pCodecCtx->rc_max_available_vbv_use = 0.8;

//   avcodec_thread_init(pCodecCtx, 10);

   //if (c->codec_id == CODEC_ID_MPEG2VIDEO)
   //{
      //c->max_b_frames = 2;  // just for testing, we also add B frames
   //}




//   if (av_set_parameters(pFormatCtx, NULL) < 0)
//   {
//      printf("Invalid output format parameters\n");
//      return false;
//   }

   // Print format settings
   ffmpeg::av_dump_format(pFormatCtx, 0, fileName.toStdString().c_str(), 1);


   // open the codec
   printf("Opening codec...\n");
   if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
   {
      printf("could not open codec\n");
      return false;
   }
   printf("Codec opened.\n");

   // Allocate memory for output
   printf("Allocating output buffer...\n");
   if(!initOutputBuf())
   {
      printf("Can't allocate memory for output bitstream\n");
      return false;
   }
   printf("Output buffer allocated.\n");

   // Allocate the YUV frame
   printf("Allocating frame...\n");
   if(!initFrame())
   {
      printf("Can't init frame\n");
      return false;
   }
   printf("Frame allocated...\n");

   printf("Open file for writing...\n");
   if (avio_open(&pFormatCtx->pb, fileName.toStdString().c_str(), AVIO_FLAG_WRITE) < 0)
   {
      printf( "Could not open '%s'\n", fileName.toStdString().c_str());
      return false;
   }
   printf("File opened.\n");

   printf("Writing header...\n");
   if( avformat_write_header(pFormatCtx, NULL) < 0){
        printf("Error writing header\n");
		return false;
   }
   printf("Header written.\n");

   ok=true;

   return true;
}

/**
   \brief Completes writing the stream, closes it, release resources.
**/
bool QVideoEncoder::close()
{
    int r = 0;
    if(!isOk()){
        printf("Encoder was not initialized to begin with.\n");
        return false;
    }
    
    // Send NULL to start flushing
    if(ffmpeg::avcodec_send_frame(pCodecCtx, NULL) < 0){
        printf("Error while initiating flush.\n");
        return false;
    }
    
    // Keeping receiving packets while this operation is successfull
    do{
        r = receiveAndWrite_p();
    } while(r == 0 || r == AVERROR(EAGAIN));
    
    if(r != AVERROR_EOF){
        printf("An error occured while receiving and writing.\n");
        return false;
    }

    printf("Write trailer.\n");
    if(av_write_trailer(pFormatCtx)!=0){
        printf("Error writing trailer.\n");
    }

   // close_video

   //avcodec_close(pVideoStream->codec);
   avcodec_close(pCodecCtx);
   freeFrame();
   freeOutputBuf();


   /* free the streams */

   for(unsigned i = 0; i < pFormatCtx->nb_streams; i++)
   {
	   //av_freep(&pFormatCtx->streams[i]->codec);
      av_freep(&pFormatCtx->streams[i]);
   }

   // Close file
   avio_close(pFormatCtx->pb);

   // Free the stream
   av_free(pFormatCtx);

   initVars();
   return true;
}


/**
   \brief Encode one frame

   The frame must be of the same size as specified in the createFile call.

   This is the standard method to encode videos with fixed frame rates.
   Each call to encodeImage adds a frame, which will be played back at the frame rate
   specified in the createFile call.
**/
int QVideoEncoder::encodeImage(const QImage &img)
{
   return encodeImage_p(img);
}
/**
   \brief Encode one frame

   The frame must be of the same size as specified in the createFile call.

   This mehtod allows to specify the presentation time stamp (pts) of the frame.
   pts is specified in multiples of 1/framerate, where framerate was specified in the createFile call
   E.g. to encode frames with a 1ms resolution: set the frame rate to 1000, and pts is the presentation
   time in milliseconds.
   pts must be monotonously increasing.
   The first frame ought to have a pts of 0 to be immediately displayed.
**/
int QVideoEncoder::encodeImagePts(const QImage &img,unsigned pts)
{
   return encodeImage_p(img,true,pts);
}


/******************************************************************************
* INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL
******************************************************************************/

void QVideoEncoder::initVars()
{
   ok=false;
   pFormatCtx=0;
   pOutputFormat=0;
   pCodecCtx=0;
   pVideoStream=0;
   pCodec=0;
   ppicture=0;
   outbuf=0;
   picture_buf=0;
   img_convert_ctx=0;
   frameId=0;
}


/**
   \brief Register the codecs
**/
bool QVideoEncoder::initCodec()
{
//   ffmpeg::avcodec_init();
   ffmpeg::av_register_all();

//   printf("License: %s\n",ffmpeg::avformat_license());
//   printf("AVCodec version %d\n",ffmpeg::avformat_version());
//   printf("AVFormat configuration: %s\n",ffmpeg::avformat_configuration());

   return true;
}

/**
   \brief Encode one frame - internal function
   custompts: true if a custom presentation time stamp  is used
   pts: presentation time stamp in milliseconds
**/
int QVideoEncoder::encodeImage_p(const QImage &img, bool custompts, unsigned pts)
{
	//printf("Encoding image start.\n");
	if(!isOk())
		return -1;
	if(custompts || pts>0){
		printf("Custompts no longer supported.\n");
	}

	//convertImage(img);       // Custom conversion routine
	//printf("Converting image.\n");
	convertImage_sws(img);     // SWS conversion



   //if(custompts)                             // Handle custom pts
   //      pCodecCtx->coded_frame->pts = pts;  // Set the time stamp



//   std::cout << "<avcodec_encode_video start>" <<std::endl;
	//printf("Sending frame.\n");
	ppicture->pts = frameId;
	++frameId;
	int got_output = ffmpeg::avcodec_send_frame(pCodecCtx, ppicture);
	if(got_output < 0){
		printf("Something went wrong sending frame\n");
        return -1;
	}

    return receiveAndWrite_p();
	//printf("Receving frame.\n");
	
	//got_output = ffmpeg::avcodec_receive_packet(pCodecCtx, &pkt);
//   std::cout << "<avcodec_encode_video end>" <<std::endl;

//   int got_output;
//   ffmpeg::avcodec_encode_video2(pCodecCtx, &pkt, ppicture, &got_output);
   //printf("Frame size: %d\n",out_size);
//   std::cout << "out_size: " << out_size <<std::endl;


   //if(custompts)                             // Handle custom pts (must set it again for the rest of the processing)
   //      pCodecCtx->coded_frame->pts = pts;  // Set the time stamp
//    do{
//    int r = ffmpeg::avcodec_receive_packet(pCodecCtx, &pkt);
//    } while(r==AVERROR(EAGAIN));
//
//   if (ffmpeg::avcodec_receive_packet(pCodecCtx, &pkt) < 0)
//   {
//       printf("Failed to receive frame.\n");
//       return -1;
//   }
//
//     //printf("c %d. pts %d. codedframepts: %ld pkt.pts: %ld\n",custompts,pts,pCodecCtx->coded_frame->pts,pkt.pts);
//       //printf("Writing frame.\n");
//
//
//      //if (pCodecCtx->coded_frame->pts != AV_NOPTS_VALUE)
//      //if (pCodecCtx->coded_frame->pts != (0x8000000000000000LL))
//   //pkt.pts=av_rescale_q(pts, pCodecCtx->time_base, pVideoStream->time_base);
////   if(pCodecCtx->coded_frame->key_frame)
////      pkt.flags |= AV_PKT_FLAG_KEY;
//
//    printf("pts %d. codec time_base: %f. video time_base: %f. pkt pts: %lld\n", pts, av_q2d(pCodecCtx->time_base), av_q2d(pVideoStream->time_base), pkt.pts);
//
//      //pkt.stream_index= pVideoStream->index;
//
////      fwrite(pkt.data, 1, pkt.size, f);
//
//       //pkt.data= outbuf;
//       //pkt.size= got_output;
//
//
////      std::cout << "<av_interleaved_write_frame start>" <<std::endl;
//
////      std::cout << "<av_interleaved_write_frame end>" <<std::endl;
//      //printf("Wrote %d\n",ret);
//
//    if(av_write_frame(pFormatCtx, &pkt)<0){
//        printf("Failed to write frame.\n");
//        return -1;
//    }
//
//    //DEBUG: Force flush stream
//    //av_write_frame(pFormatCtx, NULL);
//
////   if(av_interleaved_write_frame(pFormatCtx, &pkt)<0){
////      printf("Failed to write frame.\n");
////      return -1;
////   }
//   return 0;
}

int QVideoEncoder::receiveAndWrite_p()
{
    av_init_packet(&pkt);
    int r = ffmpeg::avcodec_receive_packet(pCodecCtx, &pkt);
    if (r < 0)
    {
        printf("Packet not ready yet.\n");
        return r;
    }

	// This code is necessary just to make things work
	ffmpeg::av_packet_rescale_ts(&pkt, pCodecCtx->time_base, pVideoStream->time_base);
	pkt.stream_index = pVideoStream->index;
	pkt.duration = av_rescale_q(1, pCodecCtx->time_base, pVideoStream->time_base);
    
    printf("codec time_base: %f. video time_base: %f. pkt pts: %lld\n", av_q2d(pCodecCtx->time_base), av_q2d(pVideoStream->time_base), pkt.pts);
    
    if(av_interleaved_write_frame(pFormatCtx, &pkt)<0){
        printf("Failed to write frame.\n");
        return -1;
    }
    
    return 0;
}



/**
  Ensures sizes are some reasonable multiples
**/
bool QVideoEncoder::isSizeValid()
{
   if(getWidth()%8)
      return false;
   if(getHeight()%8)
      return false;
   return true;
}

unsigned QVideoEncoder::getWidth()
{
   return Width;
}
unsigned QVideoEncoder::getHeight()
{
   return Height;
}
bool QVideoEncoder::isOk()
{
   return ok;
}

/**
  Allocate memory for the compressed bitstream
**/
bool QVideoEncoder::initOutputBuf()
{
   outbuf_size = getWidth()*getHeight()*3;        // Some extremely generous memory allocation for the encoded frame.
   outbuf = new uint8_t[outbuf_size];
   if(outbuf==0)
      return false;
   return true;
}
/**
  Free memory for the compressed bitstream
**/
void QVideoEncoder::freeOutputBuf()
{
   if(outbuf)
   {
      delete[] outbuf;
      outbuf=0;
   }
}

bool QVideoEncoder::initFrame()
{
   ppicture = ffmpeg::av_frame_alloc();
   if(ppicture==0)
      return false;

   ppicture->width = pCodecCtx->width;
   ppicture->height = pCodecCtx->height;
   ppicture->format = pCodecCtx->pix_fmt;

   int size = ffmpeg::av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);
   picture_buf = new uint8_t[size];
   if(picture_buf==0)
   {
      av_free(ppicture);
      ppicture=0;
      return false;
   }

   // Setup the planes
   ffmpeg::av_image_fill_arrays(ppicture->data, ppicture->linesize,
								picture_buf, pCodecCtx->pix_fmt, 
								pCodecCtx->width, pCodecCtx->height, 1);
	   //avpicture_fill((ffmpeg::AVPicture *)ppicture, picture_buf,pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

   return true;
}
void QVideoEncoder::freeFrame()
{
   if(picture_buf)
   {
      delete[] picture_buf;
      picture_buf=0;
   }
   if(ppicture)
   {
      av_free(ppicture);
      ppicture=0;
   }
}

/**
  \brief Convert the QImage to the internal YUV format

  Custom conversion - not very optimized.

**/

bool QVideoEncoder::convertImage(const QImage &img)
{
//	std::cout << "Size: " << img.width() << " " << img.height() << std::endl;
//	std::cout << "Expected: " << getWidth() << " " << getHeight() << std::endl;
	
   // Check if the image matches the size
	if((unsigned)img.width()!=getWidth() || (unsigned)img.height()!=getHeight())
   {
      printf("Wrong image size!\n");
      return false;
   }
   if(img.format()!=QImage::Format_RGB32	&& img.format() != QImage::Format_ARGB32)
   {
      printf("Wrong image format\n");
      return false;
   }

   // RGB32 to YUV420

   int size = getWidth()*getHeight();
   // Y
   for(unsigned y=0;y<getHeight();y++)
   {

      unsigned char *s = (unsigned char*)img.scanLine(y);
      unsigned char *d = (unsigned char*)&picture_buf[y*getWidth()];
      //printf("Line %d. d: %p. picture_buf: %p\n",y,d,picture_buf);

      for(unsigned x=0;x<getWidth();x++)
      {
         unsigned int r=s[2];
         unsigned int g=s[1];
         unsigned int b=s[0];

         unsigned Y = (r*2104 + g*4130 + b*802 + 4096 + 131072) >> 13;
         if(Y>235) Y=235;

         *d = Y;

         d+=1;
         s+=4;
      }
   }

   // U,V
   for(unsigned y=0;y<getHeight();y+=2)
   {
      unsigned char *s = (unsigned char*)img.scanLine(y);
      unsigned int ss = img.bytesPerLine();
      unsigned char *d = (unsigned char*)&picture_buf[size+y/2*getWidth()/2];

      //printf("Line %d. d: %p. picture_buf: %p\n",y,d,picture_buf);

      for(unsigned x=0;x<getWidth();x+=2)
      {
         // Cr = 128 + 1/256 * ( 112.439 * R'd -  94.154 * G'd -  18.285 * B'd)
         // Cb = 128 + 1/256 * (- 37.945 * R'd -  74.494 * G'd + 112.439 * B'd)

         // Get the average RGB in a 2x2 block
         int r=(s[2] + s[6] + s[ss+2] + s[ss+6] + 2) >> 2;
         int g=(s[1] + s[5] + s[ss+1] + s[ss+5] + 2) >> 2;
         int b=(s[0] + s[4] + s[ss+0] + s[ss+4] + 2) >> 2;

         int Cb = (-1214*r - 2384*g + 3598*b + 4096 + 1048576)>>13;
         if(Cb<16)
            Cb=16;
         if(Cb>240)
            Cb=240;

         int Cr = (3598*r - 3013*g - 585*b + 4096 + 1048576)>>13;
         if(Cr<16)
            Cr=16;
         if(Cr>240)
            Cr=240;

         *d = Cb;
         *(d+size/4) = Cr;

         d+=1;
         s+=8;
      }
   }
   return true;
}

/**
  \brief Convert the QImage to the internal YUV format

  SWS conversion

   Caution: the QImage is allocated by QT without guarantee about the alignment and bytes per lines.
   It *should* be okay as we make sure the image is a multiple of many bytes (8 or 16)...
   ... however it is not guaranteed that sws_scale won't at some point require more bytes per line.
   We keep the custom conversion for that case.

**/

bool QVideoEncoder::convertImage_sws(const QImage &img)
{
//	std::cout << "Size: " << img.width() << " " << img.height() << std::endl;
//	std::cout << "Expected: " << getWidth() << " " << getHeight() << std::endl;
//	std::cout << "Format: " << img.format() << " Expected: " << QImage::Format_RGB32 << " or: " << QImage::Format_ARGB32 << std::endl;
	
   // Check if the image matches the size
	if((unsigned)img.width()!=getWidth() || (unsigned)img.height()!=getHeight())
   {
      printf("Wrong image size!\n");
      return false;
   }
   if(img.format()!=QImage::Format_RGB32	&& img.format() != QImage::Format_ARGB32)
   {
      printf("Wrong image format\n");
      return false;
   }

   img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx,
												  getWidth(),
												  getHeight(),
												  ffmpeg::AV_PIX_FMT_BGRA,
												  getWidth(),
												  getHeight(),
												  ffmpeg::AV_PIX_FMT_YUV420P,
												  SWS_BICUBIC,
												  NULL,
												  NULL,
												  NULL);
   //img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx,getWidth(),getHeight(),ffmpeg::PIX_FMT_BGRA,getWidth(),getHeight(),ffmpeg::PIX_FMT_YUV420P,SWS_FAST_BILINEAR, NULL, NULL, NULL);
   if (img_convert_ctx == NULL)
   {
      printf("Cannot initialize the conversion context\n");
      return false;
   }

   uint8_t *srcplanes[3];
   srcplanes[0]=(uint8_t*)img.bits();
   srcplanes[1]=0;
   srcplanes[2]=0;

   int srcstride[3];
   srcstride[0]=img.bytesPerLine();
   srcstride[1]=0;
   srcstride[2]=0;


   ffmpeg::sws_scale(img_convert_ctx, srcplanes, srcstride,0, getHeight(), ppicture->data, ppicture->linesize);

   return true;
}

