/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_inria_genscale_dbscan_impl_plast_Request */

#ifndef _Included_org_inria_genscale_dbscan_impl_plast_Request
#define _Included_org_inria_genscale_dbscan_impl_plast_Request
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_inria_genscale_dbscan_impl_plast_Request
 * Method:    run
 * Signature: (JLorg/inria/genscale/dbscan/api/IObjectFactory;)V
 */
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_run
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_inria_genscale_dbscan_impl_plast_Request
 * Method:    cancel
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_cancel
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     org_inria_genscale_dbscan_impl_plast_Request
 * Method:    isRunning
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_isRunning
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_inria_genscale_dbscan_impl_plast_Request
 * Method:    getExecInfo
 * Signature: (J)Ljava/util/Properties;
 */
JNIEXPORT jobject JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_getExecInfo
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
