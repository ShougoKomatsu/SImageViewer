#include "stdafx.h"
#include "PanImage.h"
#include "float.h"



bool SetColorTable(CImage* imgDst, const RGBQUAD* rgbTable, const int iLength)
{
	if(imgDst==NULL){return false;}
	if(rgbTable==NULL){return false;}
	if(iLength<0){return false;}
	if(iLength>256){return false;}
	if(imgDst->IsNull()==true){return false;}

	HDC hDC = imgDst->GetDC();
	SetDIBColorTable(hDC, 0, iLength, rgbTable);
	imgDst->ReleaseDC();
	return true;
}

	bool PanImage::Convert(const VALUE_IMAGE enumMode, CImage* imgDst)
	{
		if(enumImageType==IMAGE_TYPE_CIMAGE){return CopyImage_CImage(&(this->cImage), imgDst);}

		if(enumMode == VALUE_IMAGE_RESCALE_0_TO_255)
		{
			imgDst->Create(iWidth, iHeight, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);
			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int iMax=INT_MIN;
					int iMin=INT_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							iMax=max(iMax,iImage[r*iWidth+c]);
							iMin=min(iMin,iImage[r*iWidth+c]);
						}
					}
					if(iMin==iMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, int(255 * (iImage[r*iWidth+c] - iMin)/double(iMax-iMin)) ));
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
					double dMax=DBL_MIN;
					double dMin=DBL_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							dMax=max(dMax,dImage[r*iWidth+c]);
							dMin=min(dMin,dImage[r*iWidth+c]);
						}
					}
					if(dMin==dMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, 255 * int((dImage[r*iWidth+c] - dMin)/double(dMax-dMin)) ));
						}
					}
					return true;
				}
			}
			return false;
		}
		if(enumMode == VALUE_IMAGE_CLIP_0_TO_255)
		{
			imgDst->Create(iWidth, iHeight, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, iImage[r*iWidth+c]));
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=(BYTE)min(255,max(0, dImage[r*iWidth+c]));
						}
					}
				}
			}
		}
		if(enumMode == VALUE_IMAGE_EQUALIZE)
		{
			imgDst->Create(iWidth, iHeight, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);
			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();

			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int* iIndex;
					int iThreshList[257];
					iThreshList[0]=0;
					int iLength = this->iWidth*this->iHeight;
					iIndex=new int[iLength];

					index_i(iImage, iLength, iIndex);

					int iCoutStep=(iLength-1)/256;
					for(int iLUT=1; iLUT<257; iLUT++)
					{
						int iCountThresh=iCoutStep*(iLUT);
						iThreshList[iLUT]=iImage[iIndex[iCountThresh]];
					}
					SAFE_DELETE(iIndex);

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=255;
							for(int iLUT=0; iLUT<257; iLUT++)
							{
								if ((iImage[r*iWidth+c]>=iThreshList[iLUT])&&(iImage[r*iWidth+c]<=iThreshList[iLUT+1])){pbyDataDst[r*iPitch+c]=iLUT; break;}
							}

						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=123;
						}
					}
					return true;
				}
			}
			return false;
		}
		if(enumMode == VALUE_IMAGE_HUE_CYCLIC)
		{
			imgDst->Create(iWidth, iHeight,24);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int iMax=INT_MIN;
					int iMin=INT_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							iMax=max(iMax,iImage[r*iWidth+c]);
							iMin=min(iMin,iImage[r*iWidth+c]);
						}
					}
					if(iMin==iMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							int uiValue = int(6910 * (iImage[r*iWidth+c] - iMin)/double(iMax-iMin));
							bool bRet = SetHSVValue(pbyDataDst, iPitch, 3, r, c, uiValue, 32);
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
				}
			}
		}
		if(enumMode == VALUE_IMAGE_RAINBOW)
		{
			imgDst->Create(iWidth, iHeight,24);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int iMax=INT_MIN;
					int iMin=INT_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							iMax=max(iMax,iImage[r*iWidth+c]);
							iMin=min(iMin,iImage[r*iWidth+c]);
						}
					}
					if(iMin==iMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							BYTE byValue = (BYTE)min(255,max(0, (255 * (iImage[r*iWidth+c] - iMin)/double(iMax-iMin))));
							SetRGBValue(pbyDataDst, r, c, iPitch, 3, s_byVisibleR[byValue], s_byVisibleG[byValue], s_byVisibleB[byValue]);
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
				}
			}
		}
		return false;
		}

		bool GrayToRainbow(const CImage* imgSrc, CImage* imgDst)
		{
			if(_IsImageMonochrome(imgSrc)==false){return false;}

			int iWidth = imgSrc->GetWidth();
			int iHeight = imgSrc->GetHeight();
			if(imgDst->IsNull()==false){imgDst->Destroy();}

			imgDst->Create(iWidth, iHeight, max(24, imgSrc->GetBPP()));

			int iBPP = imgDst->GetBPP();
			int iColorPitch = (iBPP==24 ? 3 : 4);
			BYTE* pbyData_src = (BYTE*)imgSrc->GetBits();
			BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
			int iPitch_src = imgSrc->GetPitch();
			int iPitch_dst = imgDst->GetPitch();

			if(imgSrc->GetBPP()==32)
			{
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						pbyData_dst[r*iPitch_dst+c*iColorPitch + 3]=pbyData_src[r*iPitch_src+c*iColorPitch + 3];
					}
				}
			}


			ImgRGB imgRGB;
			_ConvertImage(imgSrc, &imgRGB);
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					BYTE byValue = imgRGB.byImgR[r*iWidth+c];
					SetRGBValue(pbyData_dst, r, c, iPitch_dst, iColorPitch, s_byVisibleR[byValue], s_byVisibleG[byValue], s_byVisibleB[byValue]);
				}
			}
			return true;
		}

	bool PanImage::GetValue(const int r, const int c, double* dValue) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(dImage == NULL){return false;}
		if(enumImageType != IMAGE_TYPE_DIMAGE){return false;}
		*dValue = dImage[r*iWidth+c];
		return true;
	}
	bool PanImage::GetValue(const int r, const int c, int* iValue) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(iImage == NULL){return false;}
		if(enumImageType != IMAGE_TYPE_IIMAGE){return false;}
		*iValue = iImage[r*iWidth+c];
		return true;
	}
	bool PanImage::GetValue(const int r, const int c, RGBQUAD* rgbValue, BYTE* pbyAlpha, int* iBPP_out) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(enumImageType != IMAGE_TYPE_CIMAGE){return false;}


		COLORREF col = cImage.GetPixel(c,r);
		rgbValue->rgbBlue=GetBValue(col);
		rgbValue->rgbGreen=GetGValue(col);
		rgbValue->rgbRed=GetRValue(col);
		rgbValue->rgbReserved = 0;

		int iBPP =cImage.GetBPP();
		if(iBPP == 32)
		{
			BYTE* pByData = (BYTE*)cImage.GetBits();
			int iPitch = cImage.GetPitch();
			*pbyAlpha= pByData[r * iPitch +c *4+3];
		}
		else
		{
			*pbyAlpha=255;
		}

		*iBPP_out=iBPP;
		return true;
	}

	bool PanImage::Set(const IMAGE_TYPE enumImageType, const int* piImage_in, const double* pdImage_in, const int iWidth, const int iHeight, const CImage* pcImage_in, const VALUE_IMAGE enumValueImage_in, const CString sDataSource_in)
	{
		this->Init();
		switch(enumImageType)
		{
		case IMAGE_TYPE_CIMAGE:
			{
				if(pcImage_in == NULL){return false;}
				bool bRet = CopyImage_CImage(pcImage_in, &(this->cImage));
				if(bRet != true){return false;}
				this->enumImageType = IMAGE_TYPE_CIMAGE;
				this->iWidth=this->cImage.GetWidth();
				this->iHeight=this->cImage.GetHeight();
				this->enumValueImage = VALUE_IMAGE_UNDEFINED;
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				bRet = CopyImage_CImage(&(this->cImage), &((this->m_imageProcessed[0])));
				return true;
			}
		case IMAGE_TYPE_IIMAGE:
			{
				if(piImage_in == NULL){return false;}
				if(iWidth<=0){return false;}
				if(iHeight<=0){return false;}
				this->iImage=new int[iWidth*iHeight];
				this->iWidth=iWidth;
				this->iHeight=iHeight;
				this->enumImageType=IMAGE_TYPE_IIMAGE;
				this->enumValueImage = enumValueImage_in;
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						this->iImage[r*iWidth+c]=piImage_in[r*iWidth+c];
					}
				}
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				Convert(enumValueImage, &(this->cImage));
				bool bRet = CopyImage_CImage(&(this->cImage), &((this->m_imageProcessed[0])));
				return true;
			}
		case IMAGE_TYPE_DIMAGE:
			{
				if(pdImage_in == NULL){return false;}
				if(iWidth<=0){return false;}
				if(iHeight<=0){return false;}
				this->dImage=new double[iWidth*iHeight];
				this->iWidth=iWidth;
				this->iHeight=iHeight;
				this->enumImageType=IMAGE_TYPE_DIMAGE;
				this->enumValueImage = enumValueImage_in;
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						this->dImage[r*iWidth+c]=pdImage_in[r*iWidth+c];
					}
				}
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				Convert(enumValueImage, &(this->cImage));
				bool bRet = CopyImage_CImage(&(this->cImage), &((this->m_imageProcessed[0])));
				return true;
			}
		default:{return false;}
		}
		return false;
	}


CImage* PanImage::ProgressImageProcess()
{
	m_iImgProcessIndex++;
	m_iUnDoAvailableCount++;
	if(m_iUnDoAvailableCount>=MAX_IMG_PROCESS-1){m_iUnDoAvailableCount=MAX_IMG_PROCESS-1;}
	return &(m_imageProcessed[(m_iImgProcessIndex % MAX_IMG_PROCESS)]);
}
bool PanImage::UnDo()
{
	if(m_iUnDoAvailableCount<=0){return false;}
	m_iImgProcessIndex--;
	m_iUnDoAvailableCount--;
	m_iReDoAvailableCount++;
	return true;
}

bool PanImage::ReDo()
{
	if(m_iReDoAvailableCount<=0){return false;}
	m_iImgProcessIndex++;
	m_iReDoAvailableCount--;
	m_iUnDoAvailableCount++;
	return true;

}
const CImage* PanImage::GetCurrentProcess() const
{
	return &(m_imageProcessed[(m_iImgProcessIndex % MAX_IMG_PROCESS)]);
}


bool PanImage::CopyImage(const PanImage* panImgSrc)
{
	bool bRet = this->Set(panImgSrc->enumImageType, panImgSrc->iImage, panImgSrc->dImage, panImgSrc->iWidth, panImgSrc->iHeight, &panImgSrc->cImage, panImgSrc->enumValueImage, panImgSrc->sDataSource);
	if(bRet != true){return false;}
	if(panImgSrc->enumImageType != IMAGE_TYPE_CIMAGE)
	{
		bRet = CopyImage_CImage(&(panImgSrc->cImage), &(this->cImage));
		if(bRet != true){return false;}
	}
	for(int i=0; i<MAX_IMG_PROCESS; i++)
	{
		if(panImgSrc->m_imageProcessed[i].IsNull() != true)
		{
			CopyImage_CImage(&(panImgSrc->m_imageProcessed[i]), &(this->m_imageProcessed[i]));
		}

		this->m_iImgProcessIndex = panImgSrc->m_iImgProcessIndex;
		this->m_iUnDoAvailableCount = panImgSrc->m_iUnDoAvailableCount;
		this->m_iReDoAvailableCount = panImgSrc->m_iReDoAvailableCount;
	}
	return true;
}


bool ConvertStrToPanImage(const CString sImage,  const VALUE_IMAGE enumImageMode, const CString sDataSource, PanImage* panImgDst)
{
	CString sSeparator;
	int iHeight;
	int iWidth;
	bool bColored;
	bool bFloat;
	bool bRet = AnalyzeStrAsImage(sImage, &iWidth, &iHeight, &sSeparator, &bColored, &bFloat);
	if(bRet != true){return false;}
	if(bColored==true)
	{
		BYTE* byImageR;
		BYTE* byImageG;
		BYTE* byImageB;
		byImageR = new BYTE[iWidth*iHeight];
		byImageG = new BYTE[iWidth*iHeight];
		byImageB = new BYTE[iWidth*iHeight];
		bRet = ConvertStrToByte3(sImage, sSeparator, iHeight, iWidth, byImageR, byImageG, byImageB);
		CImage imgTemp;
		imgTemp.Create(iWidth, iHeight, 24);
		BYTE* pbyData=(BYTE*)imgTemp.GetBits();
		int iPitch = imgTemp.GetPitch();
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth; c++)
			{
				pbyData[r*iPitch+3*c+2]=byImageR[r*iWidth+c];
				pbyData[r*iPitch+3*c+1]=byImageG[r*iWidth+c];
				pbyData[r*iPitch+3*c+0]=byImageB[r*iWidth+c];
			}
		}
		SAFE_DELETE(byImageR);
		SAFE_DELETE(byImageG);
		SAFE_DELETE(byImageB);
		panImgDst->Set(IMAGE_TYPE_CIMAGE, NULL, NULL, iWidth, iHeight, &imgTemp, VALUE_IMAGE_CLIP_0_TO_255, sDataSource);
		return true;
	}

	if(bFloat == true)
	{
	}
	else
	{
		int* iImage;
		iImage = new int[iWidth*iHeight];
		bRet = ConvertStrToInt(sImage, sSeparator, iHeight, iWidth, iImage);
		if(bRet != true){SAFE_DELETE(iImage); return false;}

		panImgDst->Set(IMAGE_TYPE_IIMAGE,iImage,NULL, iWidth,iHeight,NULL,enumImageMode, sDataSource);
		SAFE_DELETE(iImage);
		return true;
	}
	return false;
}
	bool ImposeRGBValue(const PanImage* panImgSrc, const CImage* imgZoomed, CImage* imgDst, const int iType,const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRs_i, const int iCs_i, const int iRe_i, const int iCe_i)
	{
		if(imgZoomed != imgDst)
		{
			CopyImage_CImage(imgZoomed,imgDst);
		}
		if(dScale<dScaleThresh){return true;}


		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		int iBPP = imgDst->GetBPP();
		if(iBPP != 32){return false;}
		int iPitchDst=imgDst->GetPitch();
		int iWidthDst = imgDst->GetWidth();
		int iHeightDst = imgDst->GetHeight();
		int iRMaxDst = int(iHeightDst/dScale+2);
		int iCMaxDst = int(iWidthDst/dScale+2);

		switch(panImgSrc->GetImageType())
		{
		case IMAGE_TYPE_IIMAGE:
			{
				for(int ir_i=iRs_i; ir_i<=iRe_i; ir_i++)
				{
					int ir0_v=int(((ir_i-iRs_i)+0.5+dROffset)*dScale);
					if(ir0_v<0){continue;}
					if(ir0_v>=iHeightDst){continue;}
					for(int ic_i=iCs_i; ic_i<=iCe_i; ic_i++)
					{
						int ic0_v=int(((ic_i-iCs_i)+1+dCOffset)*dScale);
						int ic1_v=int(((ic_i-iCs_i)+0.5+dCOffset)*dScale);

						if(ic0_v<0){continue;}
						if(ic0_v>=iWidthDst){continue;}

						COLORREF col = imgDst->GetPixel(ic1_v, ir0_v);

						int iValueR =  GetRValue(col);
						int iValueG =  GetGValue(col);
						int iValueB =  GetBValue(col);
						BYTE byDot=iValueR+iValueG+iValueB<480 ? 255:0;
						int iValue;
						panImgSrc->GetValue(ir_i, ic_i,&iValue);
						if(dScale>48)
						{
							ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValue,ir0_v-8-0, ic0_v-2-16, byDot);
						}
						else
						{
							ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValue,ir0_v-4-0, ic0_v-2-8, byDot);
						}
					}
				}
				return true;
			}
		case IMAGE_TYPE_CIMAGE:
			{

				for(int ir_i=iRs_i; ir_i<=iRe_i; ir_i++)
				{
					int ir0_v=int(((ir_i-iRs_i)+0.5+dROffset)*dScale);
					if(ir0_v<0){continue;}
					if(ir0_v>=iHeightDst){continue;}
					for(int ic_i=iCs_i; ic_i<=iCe_i; ic_i++)
					{
						int ic0_v=int(((ic_i-iCs_i)+1+dCOffset)*dScale);

						if(ic0_v<0){continue;}
						if(ic0_v>=iWidthDst){continue;}

						COLORREF col = panImgSrc->GetCImage()->GetPixel(ic_i, ir_i);
						int iValueR =  GetRValue(col);
						int iValueG =  GetGValue(col);
						int iValueB =  GetBValue(col);

						col = (panImgSrc->GetCurrentProcess())->GetPixel(ic_i, ir_i);
						int iValue =  GetRValue(col);
						iValue +=  GetGValue(col);
						iValue +=  GetBValue(col);
						BYTE byDot=iValue<480 ? 255:0;

						if(dScale>48)
						{
							if((iValueR==iValueG) && (iValueR==iValueB))
							{
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-8-0, ic0_v-2-16, byDot);
							}
							else
							{
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueR,ir0_v-8-16, ic0_v-2-16, byDot);
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-8-0, ic0_v-2-16, byDot);
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueB,ir0_v-8+16, ic0_v-2-16, byDot);
							}
						}
						else
						{
							if((iValueR==iValueG) && (iValueR==iValueB))
							{
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-4-0, ic0_v-2-8, byDot);
							}
							else
							{
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueR,ir0_v-4-8, ic0_v-2-8, byDot);
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-4-0, ic0_v-2-8, byDot);
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueB,ir0_v-4+8, ic0_v-2-8, byDot);
							}
						}
					}
				}
				return true;
			}
		}
		return false;
	}