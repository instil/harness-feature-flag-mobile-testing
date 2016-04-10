/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.8
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_Surge_WRAP_H_
#define SWIG_Surge_WRAP_H_

class SwigDirector_RtspClientDelegate : public Surge::RtspClientDelegate, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_RtspClientDelegate(JNIEnv *jenv);
    virtual ~SwigDirector_RtspClientDelegate();
    virtual void ClientDidTimeout();
    virtual void Payload(char const *frameBuffer, size_t length);
public:
    bool swig_overrides(int n) {
      return (n < 2 ? swig_override[n] : false);
    }
protected:
    Swig::BoolArray<2> swig_override;
};

class SwigDirector_LoggingDelegate : public SurgeUtil::LoggingDelegate, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_LoggingDelegate(JNIEnv *jenv);
    virtual ~SwigDirector_LoggingDelegate();
    virtual void info(char const *message) const;
    virtual void error(char const *message) const;
    virtual void debug(char const *message) const;
    virtual void fatal(char const *message) const;
    virtual void warn(char const *message) const;
    virtual void trace(char const *message) const;
public:
    bool swig_overrides(int n) {
      return (n < 6 ? swig_override[n] : false);
    }
protected:
    Swig::BoolArray<6> swig_override;
};


#endif
