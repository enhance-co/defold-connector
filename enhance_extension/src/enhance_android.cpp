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
#include "FglEnhanceAndroid.h"
#include "FglEnhancePlusAndroid.h"
#include "FglEnhanceInAppPurchasesAndroid.h"
#include "enhance_internal.h"

static EnhanceListener *g_pInterstitialCompleted;
static EnhanceListener *g_pCurrencyGranted;
static EnhanceListener *g_pRewardGranted;
static EnhanceListener *g_pRewardDeclined;
static EnhanceListener *g_pRewardUnavailable;
static EnhanceListener *g_pPermissionGranted;
static EnhanceListener *g_pPermissionRefused;

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

		/*
		case ENHANCE_EVENT_PURCHASE_SUCCEEDED:
			if (g_callback_onPurchaseSuccess != NULL)
				g_callback_onPurchaseSuccess();
			break;

		case ENHANCE_EVENT_PURCHASE_FAILED:
			if (g_callback_onPurchaseFailed != NULL)
				g_callback_onPurchaseFailed();
			break;

		case ENHANCE_EVENT_CONSUME_SUCCEEDED:
			if (g_callback_onConsumeSuccess != NULL)
				g_callback_onConsumeSuccess();
			break;

		case ENHANCE_EVENT_CONSUME_FAILED:
			if (g_callback_onConsumeFailed != NULL)
				g_callback_onConsumeFailed();
			break; 
		*/
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
JNIEXPORT bool Defold_Enhance_isSpecialOfferReady(void) {
	return enhanceJniCallVoidRetBool(ENHANCE_JAVA_CLASS, "isSpecialOfferReady");
}

/**
 * Show special offer
 */
JNIEXPORT void Defold_Enhance_showSpecialOffer(void) {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "showSpecialOffer");
}

/**
 * Check if an offerwall is ready to show
 * 
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isOfferwallReady(void) {
	return enhanceJniCallVoidRetBool(ENHANCE_JAVA_CLASS, "isOfferwallReady");
}

/**
 * Show offerwall
 */
JNIEXPORT void Defold_Enhance_showOfferwall (void) {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "showOfferwall");
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
 * Check if an overlay is ready to show
 * 
 * @return true if ready, false if not
 */
JNIEXPORT bool Defold_Enhance_isOverlayAdReady(void) {
   return enhanceJniCallVoidRetBool(ENHANCE_JAVA_CLASS, "isOverlayAdReady");
}

/**
 * Show overlay ad
 *
 * @param position ad position (top or bottom)
 */
JNIEXPORT void Defold_Enhance_showOverlayAd(const char *pszPosition) {
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "showOverlayAdWithPositionJNI", pszPosition);
}

/**
 * Show overlay ad
 *
 * @param x X coordinate of top, left of rectangle to show ad at, in screen coordinates
 * @param y Y coordinate of top, left of rectangle to show ad at, in screen coordinates
 * @param width width of rectangle to show ad at, in screen coordinates
 * @param height height of rectangle to show ad at, in screen coordinates
 */
JNIEXPORT void FglEnhance_showOverlayAdWithRect(int x, int y, int width, int height) {
	enhanceJniCallIntIntIntIntRetVoid(ENHANCE_JAVA_CLASS, "showOverlayAdWithRect", x, y, width, height);
}

/**
 * Hide overlay ad, if one is showing
 */
JNIEXPORT void Defold_Enhance_hideOverlayAd() {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "hideOverlayAd");
}

/**
 * Show a new ad in the overlay, if one is showing
 */
JNIEXPORT void FglEnhance_refreshOverlayAd() {
	enhanceJniCallVoidRetVoid(ENHANCE_JAVA_CLASS, "refreshOverlayAd");
}

/**
* Request permission from the user to show local notifications
*
* @param callback_onPermissionGranted function called when the permission is granted
* @param callback_onPermissionRefused function called when the permission is declined
*/
JNIEXPORT void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted,
															 EnhanceListener *pRefused) {
   if (g_pPermissionGranted) 
      g_pPermissionGranted->callWithNoParam();
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
 * Log debug message to the console
 * 
 * @param tag tag to use
 * @param message message to log
 */

JNIEXPORT void FglEnhancePlus_logMessage(const char *str_tag, const char *str_message) {
	enhanceJniCallStrStrRetVoid(ENHANCEPLUS_JAVA_CLASS, "logMessage", str_tag, str_message);
}

/**
 * Get app's version name
 * 
 * @param out_buffer output buffer for app's version name string
 * @param out_buffer_size size of output buffer in bytes
 */

JNIEXPORT void FglEnhancePlus_getAppVersionName(char *out_buffer, size_t out_buffer_size) {
	enhanceJniCallVoidRetString(ENHANCEPLUS_JAVA_CLASS, "getAppVersionName", out_buffer, out_buffer_size);
}

/**
 * Get app's version code
 * 
 * @return version code
 */

JNIEXPORT int FglEnhancePlus_getAppVersionCode() {
	return enhanceJniCallVoidRetInt(ENHANCEPLUS_JAVA_CLASS, "getAppVersionCode");
}

/**
 * Check the app's target market
 * 
 * @param out_buffer output buffer for target market string (such as googleplay or amazon)
 * @param out_buffer_size size of output buffer in bytes
 */

JNIEXPORT void FglEnhancePlus_getMarket(char *out_buffer, size_t out_buffer_size) {
	enhanceJniCallVoidRetString(ENHANCEPLUS_JAVA_CLASS, "getMarket", out_buffer, out_buffer_size);
}

/**
 * Get arbitrary configuration variable, as passed in by Enhance
 * 
 * @param str_varName variable name (case-sensitive)
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */

JNIEXPORT void FglEnhancePlus_getConfigVariable(const char *str_varName, char *out_buffer, size_t out_buffer_size) {
	enhanceJniCallStrRetString(ENHANCEPLUS_JAVA_CLASS, "getConfigVariable", str_varName, out_buffer, out_buffer_size);
}

/**
 * Update the value of a particular SDK key
 *
 * @param str_id ID of sdk to set key for
 * @param str_key key to set
 * @param str_value new value
 */

JNIEXPORT void FglEnhancePlus_setSdkValue(const char *str_id, const char *str_key, const char *str_value) {
   enhanceJniCallStrStrStrRetVoid(ENHANCEPLUS_JAVA_CLASS, "setSdkValue", str_id, str_key, str_value);
}

/**
 * Set current screen
 *
 * @param str_name name of the currently displayed screen
 */

JNIEXPORT void FglEnhancePlus_setCurrentScreen(const char *str_name) {
   enhanceJniCallStrRetVoid(ENHANCEPLUS_JAVA_CLASS, "setCurrentScreen", str_name);
}

/**
 * Get id of sdk that granted the most recent reward
 *
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */
JNIEXPORT void FglEnhancePlus_getSdkIdForLastReward(char *out_buffer, size_t out_buffer_size) {
   enhanceJniCallVoidRetString(ENHANCEPLUS_JAVA_CLASS, "getSdkIdForLastReward", out_buffer, out_buffer_size);
}


/**
 * Check if in-app purchases are supported
 * 
 * @return true if supported, false if not


JNIEXPORT bool FglEnhanceInAppPurchases_isSupported() {
	return enhanceJniCallVoidRetBool(ENHANCEINAPPURCHASES_JAVA_CLASS, "isSupported");
}
 */
/**
 * Attempt purchase
 * 
 * @param str_sku product to purchase
 * @param callback_onPurchaseSuccess function called when the purchase is successful
 * @param callback_onPurchaseFailed function called when the purchase is declined


JNIEXPORT void FglEnhanceInAppPurchases_attemptPurchase(const char *str_sku, void(*callback_onPurchaseSuccess)(void),
														void(*callback_onPurchaseFailed)(void)) {
	g_callback_onPurchaseSuccess = callback_onPurchaseSuccess;
	g_callback_onPurchaseFailed = callback_onPurchaseFailed;
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "attemptPurchaseJNI", str_sku);
}
 */
/**
 * Consume purchase
 * 
 * @param str_sku product to consume
 * @param callback_onConsumeSuccess function called when the purchase is successful
 * @param callback_onConsumeFailed function called when the purchase is declined


JNIEXPORT void FglEnhanceInAppPurchases_consume(const char *str_sku, void(*callback_onConsumeSuccess)(void),
												void(*callback_onConsumeFailed)(void)) {
	g_callback_onConsumeSuccess = callback_onConsumeSuccess;
	g_callback_onConsumeFailed = callback_onConsumeFailed;
	enhanceJniCallStrRetVoid(INTERNAL_JAVA_CLASS, "consumeJNI", str_sku);
}
 */
/**
 * Get product price
 * 
 * @param str_sku product to get price of
 * @param str_default_price default price, in case the information isn't available
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */

JNIEXPORT void FglEnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price, char *out_buffer, size_t out_buffer_size) {
	enhanceJniCallStrStrRetString(ENHANCEINAPPURCHASES_JAVA_CLASS, "getDisplayPrice", str_sku, str_default_price, out_buffer, out_buffer_size);
}

/**
 * Check if product is owned
 * 
 * @param str_sku product to check
 *
 * @return true if owned, false if not
 */

JNIEXPORT bool FglEnhanceInAppPurchases_isItemOwned(const char *str_sku) {
	return enhanceJniCallStrRetBool(ENHANCEINAPPURCHASES_JAVA_CLASS, "isItemOwned", str_sku);
}

/**
 * Check number of copies owned of the product
 * 
 * @param str_sku product to check
 *
 * @return number of copies owned
 */

JNIEXPORT int FglEnhanceInAppPurchases_getOwnedItemCount(const char *str_sku) {
	return enhanceJniCallStrRetInt(ENHANCEINAPPURCHASES_JAVA_CLASS, "getOwnedItemCount", str_sku);
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
