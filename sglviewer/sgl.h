/*****************************
Copyright 2017 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
********************************/
//IF YOU USE THIS CODE, PLEASE CITE
//"Automatic generation and detection of highly reliable fiducial markers under occlusion"
//http://www.sciencedirect.com/science/article/pii/S0031320314000235
//AND
//"Generation of fiducial marker dictionaries using mixed integer linear programming"
//http://www.sciencedirect.com/science/article/pii/S0031320315003544

#ifndef _SGL_H
#define _SGL_H
#include <memory>
#include <cstring>
#include <vector>
#include <cmath>
#include <iostream>
#include <vector>
//Simplest graphics library
//Version 1.0.1
//Author. Rafael Muñoz Salinas (rmsalinas@uco.es) 2017

//Extremmely simple yet efficient device independent graphic library for points and lines
namespace sgl{
struct Point3{
    inline Point3( ){ }
    inline Point3(float X,float Y,float Z){x=X;y=Y;z=Z;}
    inline float norm()const{return sqrt( (x*x)+(y*y)+(z*z));}
    inline Point3 operator-(const Point3 &p)const{return Point3( x-p.x, y-p.y,z-p.z);}
    inline Point3 operator+(const Point3 &p)const{return Point3( x+p.x, y+p.y,z+p.z);}
    inline Point3 operator*(float v)const{return Point3( x*v,y*v,z*v);}
    float x,y,z;
    friend std::ostream & operator<<(std::ostream &str, const Point3 &p){
        str<<"["<<p.x<<" "<<p.y<<" "<<p.z<<"]";
        return str;
    }
};
struct Point2{
    Point2( ){ }
    Point2(float X,float Y){x=X;y=Y;}
    float x,y;

    friend std::ostream & operator<<(std::ostream &str, const Point2 &p){
        str<<"["<<p.x<<" "<<p.y<<"]";
        return str;
    }
};
struct Matrix44{

    inline Matrix44 (){float m[16]={1,0,0,0, 0,1,0,0 ,0,0,1,0 ,0,0,0,1};std::memcpy(m44,m,16*sizeof(float));}
    inline Matrix44 (float m[16]){ std::memcpy(m44,m,16*sizeof(float));}
    inline const Matrix44&operator=(const Matrix44&M){std::memcpy(m44,M.m44,16*sizeof(float));return *this;}
    inline   void  rotateX(float rads){
        float c=cos(rads),s=sin(rads);
        float m[16]={1,0,0,0, 0,c,-s,0, 0,s,c,0, 0,0,0,1};
        (*this)= Matrix44(m)*(*this);
    }
    inline   void  rotateY(float rads){
        float c=cos(rads),s=sin(rads);
        float m[16]={c,0,s,0, 0,1,0,0 ,-s,0,c,0 ,0,0,0,1};
        (*this)= Matrix44(m)*(*this);
    }

    inline   void  rotateZ(float rads){
        float c=cos(rads),s=sin(rads);
        float m[16]={c,-s,0,0, s,c,0,0 ,0,0,1,0 ,0,0,0,1};
        (*this)= Matrix44(m)*(*this);
    }
    inline   void  translate(const Point3 &p){
        float m[16]={1,0,0,p.x, 0,1,0,p.y ,0,0,1,p.z ,0,0,0,1};
        (*this)= Matrix44(m)*(*this);
     }
    inline   Point3 operator*(const Point3 &p){    return Point3( p.x*m44[0]+ p.y*m44[1]+p.z*m44[2]+m44[3], p.x*m44[4]+ p.y*m44[5]+p.z*m44[6]+m44[7],p.x*m44[8]+ p.y*m44[9]+p.z*m44[10]+m44[11] );}
    inline  Matrix44 operator*(const Matrix44 &M){
        Matrix44 res;
        for(int i=0;i<3;i++)
            for(int j=0;j<4;j++){
                res(i,j)=0;
                for(int k=0;k<4;k++)
                    res(i,j)+= at(i,k)*M(k,j);
            }
        return res;
    }


    //access to elements
    inline float & operator()(int r,int c){return m44[r*4+c];}
    inline float   operator()(int r,int c)const{return m44[r*4+c];}
    inline float & at(int r,int c){return m44[r*4+c];}


    friend std::ostream & operator<<(std::ostream &str, const Matrix44 &m){
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++)
                str<<m(i,j)<<" ";
            str<<std::endl;
        }
        return str;
    }

    float m44[16];
};


struct Color{

    Color(){
    }
    Color(unsigned char  r,unsigned char  g,unsigned char b,unsigned char a=255){
        rgb[0]=r;
        rgb[1]=g;
        rgb[2]=b;
        rgb[3]=a;
    }
    inline unsigned char &operator[](int i){return rgb[i];}
    unsigned char rgb[4];
};


class Scene{

    struct ColorBuffer{


        ColorBuffer(){deallocate();}

        void create(int w,int h,int nchannels){
            deallocate();
            ptr=new uint8_t[w*h*nchannels];
            mustFreeBuffer=true;
            _w=w;
            _h=h;
            _nchannels=nchannels;
        }
        void set(int w,int h,int nchannels, uint8_t *externalbuffer ){
            ptr=externalbuffer;
            mustFreeBuffer=false;
            _w=w;
            _h=h;
            _nchannels=nchannels;
        }

        void deallocate(){
            if (ptr!=nullptr && mustFreeBuffer) delete ptr;
            ptr=nullptr;
            mustFreeBuffer=false;
        }/*
        inline uint8_t * ptr( int y){
            assert( y>0 && y<_h);
            return ptr+ y*_w*_nchannels;
        }

        inline uint8_t * ptr( int x,int y){
            assert(x>0 && y>0 && x<_w && y<_h);
            return ptr+ y*_w*_nchannels+ x*_nchannels;
        }*/
        inline uint8_t operator[](uint32_t i){return *(ptr+i*_nchannels);}
        inline void set(uint32_t i,const Color &c){memcpy(ptr+i*_nchannels,&c,_nchannels);}
        inline void set(uint32_t x,uint32_t y,const Color &c){memcpy(ptr+ (y*_w+x)*_nchannels,&c,_nchannels);}

        int _w,_h,_nchannels=3;
        uint8_t *ptr=nullptr;
        bool mustFreeBuffer=false;

    };

    Matrix44 _viewMatrix;
    std::vector<Matrix44> modelMatrices;
    Matrix44 _cameraMatrix;
    Matrix44 TM;//transform from global coordinates to camera ones
    ColorBuffer colorbuffer;

    float _focal;
    int _width,_height;

    inline void set(void *ptr,Color c,int n){memcpy(ptr,&c,n);}
public:
    Scene(){
        modelMatrices.resize(1);
        modelMatrices[0]= Matrix44();//set identity
        TM=_viewMatrix;
    }

    ~Scene(){ }

    //sets external buffer

    inline void setCameraParams(float focal,int width,int height,int nchannels,void *external_buffer=0){
        _focal=focal;
        _width=width;
        _height=height;
        _cameraMatrix(0,0)=_focal*float(_width);
        _cameraMatrix(1,1)=_focal*float(_width);
        _cameraMatrix(0,2)=float(_width)/2.;
        _cameraMatrix(1,2)=float(_height)/2.;


        if(external_buffer!=0) colorbuffer.set(width,height,nchannels,(uint8_t*)external_buffer);
        else colorbuffer.create(width,height,nchannels);
    }



    //this will erase the transform matrix
    inline void setViewMatrix(const Matrix44&vp){_viewMatrix=vp; TM=_viewMatrix*modelMatrices.back();}

    inline void setModelMatrix(const Matrix44 &M=Matrix44()){
        modelMatrices.resize(1);
        modelMatrices[0]=M;
        TM=_viewMatrix*modelMatrices.back();
    }
    inline void pushModelMatrix(const Matrix44 &M=Matrix44()){
        modelMatrices.push_back(modelMatrices.back()*M);
        TM=_viewMatrix*modelMatrices.back();
    }
    inline void popModelMatrix(){
        if (modelMatrices.size()>1){
            modelMatrices.pop_back();
            TM=_viewMatrix*modelMatrices.back();
        }
    }



    inline void clear(const Color &backgroundColor){

        const int size=_width*_height;
        for(int i=0;i<size;i++) colorbuffer.set(i,backgroundColor);
      }
    //returns current view point
    inline Matrix44& getViewMatrix(){return _viewMatrix;}
    inline Matrix44& getModelMatrix(){return modelMatrices.back();}

    //draws a 3d point
    inline void drawPoint(const Point3 &p,const Color &c,int size=1){  drawPoint(&p,c,size);}
    inline void drawPoints(const std::vector<Point3> &vp,const Color &c,int size=1){
        (void)size;
        for(size_t i=0;i<vp.size();i++){
            Point3 pres=TM*vp[i];
            if (pres.z>0){
                pres=_cameraMatrix*pres;
                float invz=1./pres.z;
                pres.x*=invz;pres.y*=invz;
                if ( pres.x>= 0 &&  pres.x<_width && pres.y>=0 && pres.y<_height)
                    colorbuffer.set(pres.x,pres.y,c);
            }
        }

    }

    inline void drawPoints(const std::vector<Point3> &vp,const std::vector<Color> &vc,int size=1){
        (void)size;
        auto M=_cameraMatrix*TM;
        for(size_t i=0;i<vp.size();i++){
            Point3 pres=M*vp[i];
            if (pres.z>0){
                pres=_cameraMatrix*pres;
                float invz=1./pres.z;
                pres.x*=invz;pres.y*=invz;
                if ( pres.x>= 0 &&  pres.x<_width && pres.y>=0 && pres.y<_height)
                    colorbuffer.set(pres.y,pres.x,vc[i]);
            }
        }
    }
    inline void drawPoint(const Point3 *p,const Color &c,int size=1){
        Point2 pres;
        Point3 p3d=TM*(*p);
        if ( p3d.z<0 ) return;
        if ( project(p3d,pres)){
            int ix=pres.x+0.5;
            int iy=pres.y+0.5;
             switch (size) {
                case 1:
                if ( ix>=0 && ix<_width && iy>=0 && iy<_height){
                    colorbuffer.set(ix,iy,c);
                    }
                break;
                case 2:
                if ( ix> 0 && ix<_width-1 && iy>0 && iy<_height-1){
                    int stx=(ix-1);
                    int sty=(iy-1)*_width;
                    colorbuffer.set (sty+stx,c);colorbuffer.set (sty+stx-1,c);colorbuffer.set (sty+stx+1,c);
                    sty=(int(pres.y))*_width;
                    colorbuffer.set (sty+stx,c);colorbuffer.set (sty+stx-1,c);colorbuffer.set (sty+stx+1,c);
                    sty=(int(pres.y+1))*_width;
                    colorbuffer.set (sty+stx,c);colorbuffer.set (sty+stx-1,c);colorbuffer.set (sty+stx+1,c);
                }break;

            }
        }
            (void)size;
    }

    inline void drawLine(const Point3 &p1,const Point3 &p2,const Color &color,int size=1){drawLine(&p1,&p2,color,size);}
    inline void drawLine(const Point3 *p1,const Point3 *p2,const Color &color,int size=1){
        Point3 p1t=TM*(*p1); if ( p1t.z<0 ) return;
        Point3 p2t=TM*(*p2);  if(p2t.z<0) return;//check that both are in front of camera(otherwise, do not draw)
        Point2 pr1,pr2;
        if(! project(p1t,pr1))return;
        if(! project(p2t,pr2))return;
        drawline(pr1,pr2,color,size);//project line bweten projected points
    }

    //returns the internal frame buffer
    inline unsigned char* getBuffer()const{return (unsigned char*)colorbuffer.ptr;}

    inline int getWidth()const{return _width;}
    inline int getHeight()const{return _height;}
private:


    inline bool project(const Point3 &p,Point2 &res){
        Point3 pres=_cameraMatrix*(p);
        if(pres.z==0) return false;
        float invz=1./pres.z;
        res.x=invz*pres.x+0.5; res.y=invz*pres.y+0.5;
        return true;
    }

    //Bresenham's algorithm

    inline bool inLimits(int x,int y){
        if (x<0)return false;
        if (y<0)return false;
        if(x>=_width)return false;
        if(y>=_height)return false;
        return true;
    }

    void drawline(Point2 start, Point2 end,  const Color& color ,int size=1)
    {


        (void)size;
        int x0=start.x,y0=start.y,x1=end.x,y1=end.y;

        if (!inLimits(x0,y0) && !inLimits(x1,y1) ) return;
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1, sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        while (true)
        {
            if(y0>=0 && x0>=0 && y0<_height && x0<_width)   colorbuffer.set(x0,y0,color);
            if (x0 == x1 && y0 == y1) return;
            int e2 = (err << 1);
            if (e2 > -dy){err -= dy;x0 += sx;}
            if (e2 < dx){err += dx;y0 += sy;}
        }
    }
};

}


#endif
