#if defined(DM_PLATFORM_ANDROID)
/**
 * JNI interface for the Enhance connector - implementation
 */

#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <wchar.h>
#include <sys/stat.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>
#include "enhance_internal.h"

static EnhanceListener *g_pInterstitialCompleted;
static EnhanceListener *g_pCurrencyGranted;
static EnhanceListener *g_pRewardGranted;
static EnhanceListener *g_pRewardDeclined;
static EnhanceListener *g_pRewardUnavailable;
static EnhanceListener *g_pPermissionGranted;
static EnhanceListener *g_pPermissionRefused;
static EnhanceListener *g_callback_onPurchaseSuccess;
static EnhanceListener *g_callback_onPurchaseFailed;
static EnhanceListener *g_callback_onConsumeSuccess;
static EnhanceListener *g_callback_onConsumeFailed;

/** Java class names of the Enhance connector */
#define INTERNAL_JAVA_CLASS "com.fgl.enhance.connector.Internal"
#define ENHANCE_JAVA_CLASS "com.fgl.enhance.connector.FglEnhance"
#define ENHANCEPLUS_JAVA_CLASS "com.fgl.enhance.connector.FglEnhancePlus"
#define ENHANCEINAPPURCHASES_JAVA_CLASS "com.fgl.enhance.connector.FglEnhanceInAppPurchases"

/* Log info messages to the adb console */
#define LOGI(...) ((void)__android_log_print (ANDROID_LOG_INFO, "fgl.sdk.enhance", __VA_ARGS__))

extern "C" {
   /* Native callbacks for the java connector */
   JNIEXPORT void JNICALL Java_com_fgl_enhance_connector_fglenhancejni_onEnhanceEvent(JNIEnv * env, jobject obj, int nSdkEvent, jobject strParam, int intParam0, int intParam1, int intParam2);
};

/** Java VM instance */
static JavaVM *g_jvm = NULL;

/** Mutex protecting access to the list of received Enhance events */
static pthread_mutex_t g_nEventsMutex;

/** Event type */

enum EventType {
	ENHANCE_EVENT_NONE = 0,
	ENHANCE_EVENT_REWARD_GRANTED = 1,
	ENHANCE_EVENT_REWARD_DECLINED = 2,
	ENHANCE_EVENT_REWARD_UNAVAILABLE = 3,
	ENHANCE_EVENT_CURRENCY_GRANTED = 4,
	ENHANCE_EVENT_PURCHASE_SUCCEEDED = 5,
	ENHANCE_EVENT_PURCHASE_FAILED = 6,
	ENHANCE_EVENT_CONSUME_SUCCEEDED = 7,
	ENHANCE_EVENT_CONSUME_FAILED = 8,
   ENHANCE_EVENT_INTERSTITIAL_COMPLETED = 9,
};

/** Received event */

class received_event_t {
public:
   /** Next in list */
   received_event_t *next;

   /** Previous in list */
   received_event_t *prev;

   /** Event type */
   int nEventType;

   /** String parameter, NULL for none */
   char *lpszStrParam;

   /** Integer parameters */
   int intParam[3];
};

/** First entry in list of received events */
static received_event_t *g_firstEvent = NULL;

/** Last entry in list of received events */
static received_event_t *g_lastEvent = NULL;

static jclass GetClass(JNIEnv* env, const char* classname)
{
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}

static JNIEnv* Attach()
{
    JNIEnv* env;
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->AttachCurrentThread(&env, NULL);
    return env;
}

static bool Detach(JNIEnv* env)
{
    bool exception = (bool) env->ExceptionCheck();
    env->ExceptionClear();
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->DetachCurrentThread();
    return !exception;
}

namespace {
struct AttachScope
{
    JNIEnv* m_Env;
    AttachScope() : m_Env(Attach())
    {
    }
    ~AttachScope()
    {
        Detach(m_Env);
    }
};
}

/** Native callback invoked by the Enhance java connector when an event occurs */

JNIEXPORT void JNICALL Java_com_fgl_enhance_connector_fglenhancejni_onEnhanceEvent(JNIEnv * env, jobject obj, int nEventType, jobject strParam, int intParam0, int intParam1, int intParam2) {
   JNIEnv *threadEnv = NULL;
   const char *lpJavaStr = NULL;
   received_event_t *lpNewEvent;

   threadEnv = env;
   
   if (strParam)
	  lpJavaStr = threadEnv->GetStringUTFChars ((jstring) strParam, NULL);

   /* Add event to list, with thread safety */
   
   pthread_mutex_lock (&g_nEventsMutex);

   lpNewEvent = new received_event_t;
   lpNewEvent->next = NULL;
   lpNewEvent->prev = g_lastEvent;
   lpNewEvent->nEventType = nEventType;
   lpNewEvent->lpszStrParam = NULL;
   if (lpJavaStr)
      lpNewEvent->lpszStrParam = strdup (lpJavaStr);
   lpNewEvent->intParam[0] = intParam0;
   lpNewEvent->intParam[1] = intParam1;
   lpNewEvent->intParam[2] = intParam2;
   if (g_lastEvent)
      g_lastEvent->next = lpNewEvent;
   g_lastEvent = lpNewEvent;
   if (lpNewEvent->prev == NULL)
      g_firstEvent = lpNewEvent;

   pthread_mutex_unlock (&g_nEventsMutex);

   if (strParam)
	threadEnv->ReleaseStringUTFChars ((jstring) strParam, lpJavaStr);
}

/**
 * Call Enhance connector java method that takes no parameters and returns nothing
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 */

void enhanceJniCallVoidRetVoid(const char *lpszClassName, const char *lpszMethodName) {
   JNIEnv *threadEnv = NULL;
   jclass enhanceJNIClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "()V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (enhanceJNIClass, methodId);
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
}

/**
 * Call Enhance connector java method that takes no parameters and returns a bool value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 *
 * @return bool value
 */

bool enhanceJniCallVoidRetBool(const char *lpszClassName, const char *lpszMethodName) {
   LOGI ("Enhance: enhanceJniCallVoidRetBool");
   JNIEnv *threadEnv = NULL;
   jclass enhanceJNIClass;
   jmethodID methodId;
   bool bResult = false;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   LOGI ("Enhance: enhanceJniCallVoidRetBool - GetClass(env, lpszClassName); passed");
   
   if (enhanceJNIClass) {
     LOGI ("Enhance: enhanceJniCallVoidRetBool - found class");
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "()Z");
      if (methodId != 0) {
      LOGI ("Enhance: enhanceJniCallVoidRetBool - found method");
         jboolean javaResult = threadEnv->CallStaticBooleanMethod (enhanceJNIClass, methodId);
         
         if (javaResult)
            bResult = true;
         
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   return bResult;
}

/**
 * Call Enhance connector java method that takes 1 string parameter and returns nothing
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 */

void enhanceJniCallStrRetVoid(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1;
   jclass enhanceJNIClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   
   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (enhanceJNIClass, methodId, strParam1);
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam1);
}

/**
 * Call Enhance connector java method that takes 1 string parameter and returns a bool value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 *
 * @return bool value
 */

bool enhanceJniCallStrRetBool(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1;
   jclass enhanceJNIClass;
   jmethodID methodId;
   bool bResult = false;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   
   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;)Z");
      if (methodId != 0) {
         jboolean javaResult = threadEnv->CallStaticBooleanMethod (enhanceJNIClass, methodId, strParam1);
         
         if (javaResult)
            bResult = true;
         
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam1);
   return bResult;
}


/**
 * Call Enhance connector java method that takes 1 string parameter and returns an int value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 *
 * @return bool value
 */

bool enhanceJniCallStrRetInt(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1;
   jclass enhanceJNIClass;
   jmethodID methodId;
   int nResult = 0;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;
    
   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   
   enhanceJNIClass = threadEnv->FindClass (lpszClassName);
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;)I");
      if (methodId != 0) {
         jint javaResult = threadEnv->CallStaticIntMethod (enhanceJNIClass, methodId, strParam1);
         
         if (javaResult)
			 nResult = javaResult;
         
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam1);
   return nResult;
}

/**
 * Call Enhance connector java method that takes 2 string parameters and returns nothing
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 */

void enhanceJniCallStrStrRetVoid(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, const char *lpszParam2) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1, strParam2;
   jclass enhanceJNIClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   strParam2 = threadEnv->NewStringUTF (lpszParam2);
   
   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;Ljava/lang/String;)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (enhanceJNIClass, methodId, strParam1, strParam2);
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam2);
   threadEnv->DeleteLocalRef (strParam1);
}

/**
 * Call Enhance connector java method that takes 2 string parameters and 1 int parameter and returns nothing
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 */

void enhanceJniCallStrStrIntRetVoid(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, const char *lpszParam2, int nParam3) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1, strParam2;
   jclass enhanceJNIClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   strParam2 = threadEnv->NewStringUTF (lpszParam2);
   
   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;Ljava/lang/String;I)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (enhanceJNIClass, methodId, strParam1, strParam2, nParam3);
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam2);
   threadEnv->DeleteLocalRef (strParam1);
}

/**
 * Call Enhance connector java method that takes 3 string parameters and returns nothing
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 * @param lpszParam3 parameter 3
 */

void enhanceJniCallStrStrStrRetVoid(const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, const char *lpszParam2, const char *lpszParam3) {
   JNIEnv *threadEnv = NULL;
   jobject strParam1, strParam2, strParam3;
   jclass enhanceJNIClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF (lpszParam1);
   strParam2 = threadEnv->NewStringUTF (lpszParam2);
   strParam3 = threadEnv->NewStringUTF (lpszParam3);
   
   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (enhanceJNIClass, methodId, strParam1, strParam2, strParam3);
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   threadEnv->DeleteLocalRef (strParam3);
   threadEnv->DeleteLocalRef (strParam2);
   threadEnv->DeleteLocalRef (strParam1);
}

/**
 * Call java method with 4 int parameters
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 * @param lpszParam3 parameter 3
 * @param lpszParam4 parameter 4
 */

void enhanceJniCallIntIntIntIntRetVoid (const char *lpszClassName, const char *lpszMethodName, float fParam1, float fParam2, float fParam3, float fParam4) {
   JNIEnv *threadEnv = NULL;
   jclass adsorbJniClass;
   jmethodID methodId;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   adsorbJniClass = GetClass(threadEnv, lpszClassName);
   
   if (adsorbJniClass) {
      methodId = threadEnv->GetStaticMethodID (adsorbJniClass, lpszMethodName, "(IIII)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (adsorbJniClass, methodId, fParam1, fParam2, fParam3, fParam4);
         threadEnv->DeleteLocalRef (adsorbJniClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
}

/**
 * Call java method with 4 int parameters
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 * @param lpszParam3 parameter 3
 * @param lpszParam4 parameter 4
 * @param lpszParam5 parameter 5
 */

void enhanceJniCallStrIntIntIntIntRetVoid (const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, float fParam2, float fParam3, float fParam4, float fParam5) {
   JNIEnv *threadEnv = NULL;
   jclass adsorbJniClass;
   jmethodID methodId;
   jobject strParam1;
   
   AttachScope attachscope;
   threadEnv = attachscope.m_Env;

   adsorbJniClass = GetClass(threadEnv, lpszClassName);
   
   strParam1 = threadEnv->NewStringUTF (lpszParam1);

   if (adsorbJniClass) {
      methodId = threadEnv->GetStaticMethodID (adsorbJniClass, lpszMethodName, "(Ljava/lang/String;IIII)V");
      if (methodId != 0) {
         threadEnv->CallStaticVoidMethod (adsorbJniClass, methodId, strParam1, fParam2, fParam3, fParam4, fParam5);
         threadEnv->DeleteLocalRef (adsorbJniClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
}

/**
 * Call Enhance connector java method that takes no parameters and returns a string value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpBuffer output buffer for string
 * @param nBuffersize size of output buffer in bytes
 *
 * @return string value
 */

void enhanceJniCallVoidRetString (const char *lpszClassName, const char *lpszMethodName, char *lpBuffer, size_t nBufferSize) {
   JNIEnv *threadEnv = NULL;
   jclass adsorbJniClass;
   jmethodID methodId;
   
   if (!lpBuffer || nBufferSize == 0) return;

   lpBuffer[0] = 0;  /* Clear default result */
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   adsorbJniClass = GetClass(threadEnv, lpszClassName);
   
   if (adsorbJniClass) {
      methodId = threadEnv->GetStaticMethodID (adsorbJniClass, lpszMethodName, "()Ljava/lang/String;");
      if (methodId != 0) {
         jstring strResult = (jstring) threadEnv->CallStaticObjectMethod (adsorbJniClass, methodId);
         
         if (strResult) {
            const char *lpStrResult = threadEnv->GetStringUTFChars (strResult, NULL);
            
            if (lpStrResult) {
               strncpy (lpBuffer, lpStrResult, nBufferSize - 1);
			   lpBuffer[nBufferSize - 1] = 0;
            }
            
            threadEnv->ReleaseStringUTFChars (strResult, lpStrResult);
            threadEnv->DeleteLocalRef (strResult);
         }
         
         threadEnv->DeleteLocalRef (adsorbJniClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }   
}

/**
 * Call Enhance connector java method that takes takes 1 string parameter and returns a string value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpBuffer output buffer for string
 * @param nBuffersize size of output buffer in bytes
 *
 * @return string value
 */

void enhanceJniCallStrRetString (const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, char *lpBuffer, size_t nBufferSize) {
   JNIEnv *threadEnv = NULL;
   jclass adsorbJniClass;
   jmethodID methodId;
   jobject strParam1;
   
   if (!lpBuffer || nBufferSize == 0) return;

   lpBuffer[0] = 0;  /* Clear default result */
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF(lpszParam1);

   adsorbJniClass = GetClass(threadEnv, lpszClassName);
   if (adsorbJniClass) {
      methodId = threadEnv->GetStaticMethodID (adsorbJniClass, lpszMethodName, "(Ljava/lang/String;)Ljava/lang/String;");
      if (methodId != 0) {
         jstring strResult = (jstring) threadEnv->CallStaticObjectMethod (adsorbJniClass, methodId, strParam1);
         
         if (strResult) {
            const char *lpStrResult = threadEnv->GetStringUTFChars (strResult, NULL);
            
            if (lpStrResult) {
               strncpy (lpBuffer, lpStrResult, nBufferSize - 1);
			   lpBuffer[nBufferSize - 1] = 0;
            }
            
            threadEnv->ReleaseStringUTFChars (strResult, lpStrResult);
            threadEnv->DeleteLocalRef (strResult);
         }
         
         threadEnv->DeleteLocalRef (adsorbJniClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }   

   threadEnv->DeleteLocalRef(strParam1);
}

/**
 * Call Enhance connector java method that takes takes 2 string parameters and returns a string value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 * @param lpszParam1 parameter 1
 * @param lpszParam2 parameter 2
 * @param lpBuffer output buffer for string
 * @param nBuffersize size of output buffer in bytes
 *
 * @return string value
 */

void enhanceJniCallStrStrRetString (const char *lpszClassName, const char *lpszMethodName, const char *lpszParam1, const char *lpszParam2, char *lpBuffer, size_t nBufferSize) {
   JNIEnv *threadEnv = NULL;
   jclass adsorbJniClass;
   jmethodID methodId;
   jobject strParam1, strParam2;
   
   if (!lpBuffer || nBufferSize == 0) return;

   lpBuffer[0] = 0;  /* Clear default result */
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   strParam1 = threadEnv->NewStringUTF(lpszParam1);
   strParam2 = threadEnv->NewStringUTF(lpszParam2);

   adsorbJniClass = GetClass(threadEnv, lpszClassName);
   if (adsorbJniClass) {
      methodId = threadEnv->GetStaticMethodID (adsorbJniClass, lpszMethodName, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
      if (methodId != 0) {
         jstring strResult = (jstring) threadEnv->CallStaticObjectMethod (adsorbJniClass, methodId, strParam1, strParam2);
         
         if (strResult) {
            const char *lpStrResult = threadEnv->GetStringUTFChars (strResult, NULL);
            
            if (lpStrResult) {
               strncpy (lpBuffer, lpStrResult, nBufferSize - 1);
			   lpBuffer[nBufferSize - 1] = 0;
            }
            
            threadEnv->ReleaseStringUTFChars (strResult, lpStrResult);
            threadEnv->DeleteLocalRef (strResult);
         }
         
         threadEnv->DeleteLocalRef (adsorbJniClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }   

   threadEnv->DeleteLocalRef(strParam2);
   threadEnv->DeleteLocalRef(strParam1);
}

/**
 * Call Enhance connector java method that takes no parameters and returns an int value
 *
 * @param lpszClassName name of class that contains the method
 * @param lpszMethodName name of method to call
 *
 * @return int value
 */

int enhanceJniCallVoidRetInt(const char *lpszClassName, const char *lpszMethodName) {
   JNIEnv *threadEnv = NULL;
   jclass enhanceJNIClass;
   jmethodID methodId;
   int nResult = 0;
   
    AttachScope attachscope;
    threadEnv = attachscope.m_Env;

   enhanceJNIClass = GetClass(threadEnv, lpszClassName);
   if (enhanceJNIClass) {
      methodId = threadEnv->GetStaticMethodID (enhanceJNIClass, lpszMethodName, "()I");
      if (methodId != 0) {
         jint javaResult = threadEnv->CallStaticIntMethod (enhanceJNIClass, methodId);
         
         if (javaResult)
			 nResult = javaResult;
         
         threadEnv->DeleteLocalRef (enhanceJNIClass);
      }
      else {
         LOGI ("Enhance: method not found: %s", lpszMethodName);
      }
   }
   else {
      LOGI ("Enhance: java class not found");
   }
   
   return nResult;
}

/**
 * Pump Enhance events. Call this every frame from your app thread to receive callbacks
 */
JNIEXPORT void FglEnhance_pumpEvents(void) {
	int nEventType;

	do {
		int intParam[3];
		const char *lpszStrParam = NULL;
		char g_szStrParam[256];

		nEventType = ENHANCE_EVENT_NONE;
		g_szStrParam[0] = 0;

		pthread_mutex_lock(&g_nEventsMutex);

		if (g_firstEvent != NULL) {
			received_event_t *received = g_firstEvent;

			/* Pop event at the head of the list*/

			if (received->prev)
				received->prev->next = received->next;
			if (received->next)
				received->next->prev = received->prev;
			if (received == g_firstEvent)
				g_firstEvent = received->next;
			if (received == g_lastEvent)
				g_lastEvent = received->prev;

			nEventType = received->nEventType;
			intParam[0] = received->intParam[0];
			intParam[1] = received->intParam[1];
			intParam[2] = received->intParam[2];
			if (received->lpszStrParam) {
				/* String parameter received */
				strncpy(g_szStrParam, received->lpszStrParam, 255);
				g_szStrParam[255] = 0;
				lpszStrParam = g_szStrParam;
				free((void *)received->lpszStrParam);
			}
			delete received;
			received = NULL;
		}

		pthread_mutex_unlock(&g_nEventsMutex);

		switch (nEventType) {
		case ENHANCE_EVENT_INTERSTITIAL_COMPLETED:
		   dmLogInfo("EnhanceDefold[android]: onInterstitialCompleted");
		   if (g_pInterstitialCompleted)
		      g_pInterstitialCompleted->callWithNoParam();
			break;

		case ENHANCE_EVENT_REWARD_GRANTED:
			dmLogInfo("EnhanceDefold[android]: onRewardGranted");
   				if (g_pRewardGranted)
   					g_pRewardGranted->callWithIntStrParam(intParam[0], intParam[1] ? "REWARDTYPE_COINS" : "REWARDTYPE_ITEM");
			break;

		case ENHANCE_EVENT_REWARD_DECLINED:
		   dmLogInfo("EnhanceDefold[android]: onRewardDeclined");
		   if (g_pRewardDeclined) {
		      g_pRewardDeclined->callWithNoParam();
		   }
			break;

		case ENHANCE_EVENT_REWARD_UNAVAILABLE:
		   dmLogInfo("EnhanceDefold[android]: onRewardUnavailable");
		   if (g_pRewardUnavailable) {
		      g_pRewardUnavailable->callWithNoParam();
		   }
			break;

		case ENHANCE_EVENT_CURRENCY_GRANTED:
			dmLogInfo("EnhanceDefold[android]: onCurrencyGranted: %d", intParam[0]);
		   	if (g_pCurrencyGranted)
		      	g_pCurrencyGranted->callWithIntParam(intParam[0]);
			break;

		
		case ENHANCE_EVENT_PURCHASE_SUCCEEDED:
			dmLogInfo("EnhanceDefold[android]: onPurchaseSuccess");
			if (g_callback_onPurchaseSuccess)
				g_callback_onPurchaseSuccess->callWithNoParam();
			break;

		case ENHANCE_EVENT_PURCHASE_FAILED:
			dmLogInfo("EnhanceDefold[android]: onPurchaseFailed");
			if (g_callback_onPurchaseFailed)
			g_callback_onPurchaseFailed->callWithNoParam();
			break;

		case ENHANCE_EVENT_CONSUME_SUCCEEDED:
			dmLogInfo("EnhanceDefold[android]: onConsumeSuccess");
			if (g_callback_onConsumeSuccess)
				g_callback_onConsumeSuccess->callWithNoParam();
			break;

		case ENHANCE_EVENT_CONSUME_FAILED:
			dmLogInfo("EnhanceDefold[android]: onConsumeFailed");
			if (g_callback_onConsumeFailed)
				g_callback_onConsumeFailed->callWithNoParam();
			break; 
		
		}
	} while (nEventType != ENHANCE_EVENT_NONE);
}

/**
 * Check if the Enhance SDK is injected into the host app
 *
 * @param context application context
 *
 * @return true if injected, false if not
 */
JNIEXPORT bool Defold_Enhance_isEnhanced (void) {
	LOGI ("Enhance: Defold_Enhance_isEnhanced");
	return enhanceJniCallVoidRetBool(ENHANCE_JAVA_CLASS, "isEnhanced");
}

/**
 * Request permission from the user to show local notifications
 *
 * @param callback_onInterstitialCompleted function called when an interstitial is completed
 */
JNIEXPORT void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted) {
   g_pInterstitialCompleted = pCompleted;
}

/**
 * Check if an interstitial ad is ready to show
 *
 * @param str_placement placement to check readiness of, NULL for default placement
 *
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isInterstitialReady (const char *str_placement) {
	return enhanceJniCallStrRetBool(ENHANCE_JAVA_CLASS, "isInterstitialReady", str_placement);
}

/**
 * Show interstitial ad
 *
 * @param str_placement placement to show interstitial for, NULL for default placement
 */
JNIEXPORT void Defold_Enhance_showInterstitial (const char *str_placement) {
	enhanceJniCallStrRetVoid(ENHANCE_JAVA_CLASS, "showInterstitialAd", str_placement);
}

/**
 * Check if a rewarded ad is ready to show
 *
 * @param str_placement placement to check readiness of, NULL for default placement
 *
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isRewardedAdReady(const char *str_placement) {
	return enhanceJniCallStrRetBool(ENHANCE_JAVA_CLASS, "isRewardedAdReady", str_placement);
}

/**
 * Show rewarded ad
 *
 * @param str_placement placement to show rewarded ad for, NULL for default placement
 * @param callback_onRewardGranted function called when a reward is granted
 * @param callback_onRewardDeclined function called when a reward is declined
 * @param callback_onRewardUnavailable function called when a reward isn't available to show
 */
JNIEXPORT void Defold_Enhance_showRewardedAd(const char *str_placement,
											EnhanceListener *pGranted, 
											EnhanceListener *pDeclined, 
											EnhanceListener *pUnavailable) {
   	g_pRewardGranted = pGranted;
   	g_pRewardDeclined = pDeclined;
   	g_pRewardUnavailable = pUnavailable;
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "showRewardedAdFromJNI", str_placement);
}

/**
 * Set callback for receiving currency, for instance when getting asynchronous rewards from a special offer or offerwall
 *
 * @param callback_onCurrencyGranted method called when currency is granted to the user
 */
JNIEXPORT void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted) {
   g_pCurrencyGranted = pGranted;
}

/**
 * Check if a special offer is ready to show
 * 
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isSpecialOfferReady(const char *str_placement) {
	return enhanceJniCallStrRetBool(ENHANCE_JAVA_CLASS, "isSpecialOfferReady", str_placement);
}

/**
 * Show special offer
 */
JNIEXPORT void Defold_Enhance_showSpecialOffer(const char *str_placement) {
	enhanceJniCallStrRetVoid(ENHANCE_JAVA_CLASS, "showSpecialOffer", str_placement);
}

/**
 * Check if an offerwall is ready to show
 * 
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isOfferwallReady(const char *str_placement) {
	return enhanceJniCallStrRetBool(ENHANCE_JAVA_CLASS, "isOfferwallReady", str_placement);
}

/**
 * Show offerwall
 */
JNIEXPORT void Defold_Enhance_showOfferwall (const char *str_placement) {
	enhanceJniCallStrRetVoid(ENHANCE_JAVA_CLASS, "showOfferwall", str_placement);
}

/**
 * Log custom event
 *
 * @param str_eventType event type
 * @param str_paramKey optional parameter key, NULL for none
 * @param str_paramValue optional parameter value, NULL for none
 */
JNIEXPORT void Defold_Enhance_logCustomEvent(const char *str_eventType, const char *str_paramKey, const char *str_paramValue) {
	enhanceJniCallStrStrStrRetVoid(ENHANCE_JAVA_CLASS, "logEvent", str_eventType, str_paramKey, str_paramValue);
}

/**
 * Check if a fullscreen, modal ad is currently showing
 *
 * @return true if currently showing, false if not
 */
JNIEXPORT bool Defold_Enhance_isFullscreenAdShowing() {
	return enhanceJniCallVoidRetBool(ENHANCE_JAVA_CLASS, "isFullscreenAdShowing");
}

/**
 * Check if a banner is ready to show
 * 
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isBannerAdReady(const char *str_placement) {
   return enhanceJniCallStrRetBool(ENHANCE_JAVA_CLASS, "isBannerAdReady", str_placement);
}

/**
 * Show banner ad
 *
 * @param position ad position (top or bottom)
 */
JNIEXPORT void Defold_Enhance_showBannerAd(const char *str_placement, const char *pszPosition) {
	enhanceJniCallStrStrRetVoid(ENHANCE_JAVA_CLASS, "showBannerAdWithPosition", str_placement, pszPosition);
}

/**
 * Hide banner ad, if one is showing
 */
JNIEXPORT void Defold_Enhance_hideBannerAd() {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "hideBannerAd");
}

/**
 * Show a new ad in the banner, if one is showing
 */
JNIEXPORT void Defold_Enhance_refreshBannerAd() {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "refreshBannerAd");
}

/**
* Request permission from the user to show local notifications
*
* @param callback_onPermissionGranted function called when the permission is granted
* @param callback_onPermissionRefused function called when the permission is declined
*/
JNIEXPORT void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted,
															 EnhanceListener *pRefused) {
   if (pGranted) 
      pGranted->callWithNoParam();
}

/**
 * Enable local notifications
 *
 * @param str_title notification title
 * @param str_message notification message
 * @param delay notification delay in seconds
 */
JNIEXPORT void Defold_Enhance_enableLocalNotification(const char *str_title, const char *str_message, int delay){
	enhanceJniCallStrStrIntRetVoid(ENHANCE_JAVA_CLASS, "enableLocalNotification", str_title, str_message, delay);
}

/**
 * Disable local notifications
 */
JNIEXPORT void Defold_Enhance_disableLocalNotification() {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "disableLocalNotification");
}

JNIEXPORT void Defold_Enhance_pumpEvents() {
	FglEnhance_pumpEvents();
}

/**
 * Check if in-app purchases are supported
 * 
 * @return true if supported, false if not
*/

JNIEXPORT bool Defold_EnhanceInAppPurchases_isSupported() {
	return enhanceJniCallVoidRetBool(ENHANCEINAPPURCHASES_JAVA_CLASS, "isSupported");
}
 
/**
 * Attempt purchase
 * 
 * @param str_sku product to purchase
 * @param callback_onPurchaseSuccess function called when the purchase is successful
 * @param callback_onPurchaseFailed function called when the purchase is declined
*/

JNIEXPORT void Defold_EnhanceInAppPurchases_attemptPurchase(const char *str_sku, EnhanceListener *callback_onPurchaseSuccess, EnhanceListener *callback_onPurchaseFailed) {
	g_callback_onPurchaseSuccess = callback_onPurchaseSuccess;
	g_callback_onPurchaseFailed = callback_onPurchaseFailed;
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "attemptPurchaseJNI", str_sku);
}
 
/**
 * Consume purchase
 * 
 * @param str_sku product to consume
 * @param callback_onConsumeSuccess function called when the purchase is successful
 * @param callback_onConsumeFailed function called when the purchase is declined
*/

JNIEXPORT void Defold_EnhanceInAppPurchases_consume(const char *str_sku, EnhanceListener *callback_onConsumeSuccess, EnhanceListener *callback_onConsumeFailed) {
	g_callback_onConsumeSuccess = callback_onConsumeSuccess;
	g_callback_onConsumeFailed = callback_onConsumeFailed;
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "consumeJNI", str_sku);
}
 
/**
 * Get product price
 * 
 * @param str_sku product to get price of
 * @param str_default_price default price, in case the information isn't available
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */

JNIEXPORT const char* Defold_EnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price) {
    char buffer[32] = "";
    char* out_buffer = buffer;
	enhanceJniCallStrStrRetString(ENHANCEINAPPURCHASES_JAVA_CLASS, "getDisplayPrice", str_sku, str_default_price, out_buffer, 32);
	
	return out_buffer;
}

/**
 * Check if product is owned
 * 
 * @param str_sku product to check
 *
 * @return true if owned, false if not
 */

JNIEXPORT bool Defold_EnhanceInAppPurchases_isItemOwned(const char *str_sku) {
	return enhanceJniCallStrRetBool(ENHANCEINAPPURCHASES_JAVA_CLASS, "isItemOwned", str_sku);
}

/**
 * Check number of copies owned of the product
 * 
 * @param str_sku product to check
 *
 * @return number of copies owned
 */

JNIEXPORT int Defold_EnhanceInAppPurchases_getOwnedItemCount(const char *str_sku) {
	return enhanceJniCallStrRetInt(ENHANCEINAPPURCHASES_JAVA_CLASS, "getOwnedItemCount", str_sku);
}

JNIEXPORT void Defold_EnhanceInAppPurchases_restorePurchases(EnhanceListener *pSuccess, EnhanceListener *pFailed) {

}

/** Called when this native library is loaded by the app's java code */

JNIEXPORT void Defold_Enhance_Initialize () {
   /* Enable event delivery to our native callback */
   enhanceJniCallVoidRetVoid(INTERNAL_JAVA_CLASS, "startJNIInterface");
}

JNIEXPORT jint JNI_OnLoad (JavaVM *jvm, void *lpReserved) {
   JNIEnv *threadEnv = NULL;
   jclass adSdkFactoryClass;
   pthread_mutexattr_t attrs;

   LOGI ("Enhance: JNI_OnLoad");

   /* Store instance of Dalvik JVM - shared between all threads 
   //g_jvm = dmGraphics::GetNativeAndroidJavaVM();;
   //g_jvm->AttachCurrentThread(&threadEnv, NULL);*/

   /* Create mutex for event queue */   
   pthread_mutexattr_init (&attrs);
   pthread_mutexattr_settype (&attrs, PTHREAD_MUTEX_RECURSIVE);
   pthread_mutex_init (&g_nEventsMutex, &attrs);
   pthread_mutexattr_destroy (&attrs);

   return JNI_VERSION_1_6;  
}

#endif
