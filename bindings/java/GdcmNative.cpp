#include <jni.h>

#include <cstdint>
#include <exception>
#include <string>

#include "gdcm_connector.hpp"

namespace {

void throw_runtime_exception(JNIEnv* env, const char* message) {
  jclass exception_class = env->FindClass("java/lang/RuntimeException");
  if (exception_class) {
    env->ThrowNew(exception_class, message);
  }
}

std::string to_string(JNIEnv* env, jstring value) {
  if (!value) {
    throw std::runtime_error("file path must not be null");
  }

  const char* chars = env->GetStringUTFChars(value, nullptr);
  if (!chars) {
    throw std::runtime_error("unable to read Java string");
  }

  std::string result(chars);
  env->ReleaseStringUTFChars(value, chars);
  return result;
}

void put_string(JNIEnv* env, jobject map, jmethodID put_method, const char* key, const std::string& value) {
  jstring java_key = env->NewStringUTF(key);
  jstring java_value = env->NewStringUTF(value.c_str());
  env->CallObjectMethod(map, put_method, java_key, java_value);
  env->DeleteLocalRef(java_key);
  env->DeleteLocalRef(java_value);
}

jstring new_string(JNIEnv* env, const std::string& value) {
  return env->NewStringUTF(value.c_str());
}

}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_gdcm_Gdcm_readTagNative(JNIEnv* env, jclass, jstring file_path, jint group, jint element) {
  try {
    const std::string value = gdcm_connector::read_tag(
      to_string(env, file_path),
      static_cast<std::uint16_t>(group),
      static_cast<std::uint16_t>(element));
    return env->NewStringUTF(value.c_str());
  } catch (const std::exception& error) {
    throw_runtime_exception(env, error.what());
    return nullptr;
  }
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_gdcm_Gdcm_readBasicMetadataNative(JNIEnv* env, jclass, jstring file_path) {
  try {
    const gdcm_connector::BasicMetadata metadata =
      gdcm_connector::read_basic_metadata(to_string(env, file_path));

    jclass map_class = env->FindClass("java/util/HashMap");
    jmethodID constructor = env->GetMethodID(map_class, "<init>", "()V");
    jmethodID put_method = env->GetMethodID(
      map_class,
      "put",
      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jobject map = env->NewObject(map_class, constructor);
    put_string(env, map, put_method, "patient_name", metadata.patient_name);
    put_string(env, map, put_method, "patient_id", metadata.patient_id);
    put_string(env, map, put_method, "study_instance_uid", metadata.study_instance_uid);
    put_string(env, map, put_method, "series_instance_uid", metadata.series_instance_uid);
    put_string(env, map, put_method, "sop_instance_uid", metadata.sop_instance_uid);
    put_string(env, map, put_method, "modality", metadata.modality);
    put_string(env, map, put_method, "transfer_syntax", metadata.transfer_syntax);
    return map;
  } catch (const std::exception& error) {
    throw_runtime_exception(env, error.what());
    return nullptr;
  }
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_gdcm_Gdcm_readPixelsNative(JNIEnv* env, jclass, jstring file_path) {
  try {
    const gdcm_connector::PixelData pixels =
      gdcm_connector::read_pixels(to_string(env, file_path));

    jbyteArray data = env->NewByteArray(static_cast<jsize>(pixels.data.size()));
    if (!data) {
      return nullptr;
    }
    env->SetByteArrayRegion(
      data,
      0,
      static_cast<jsize>(pixels.data.size()),
      reinterpret_cast<const jbyte*>(pixels.data.data()));

    jintArray dimensions = env->NewIntArray(static_cast<jsize>(pixels.dimensions.size()));
    if (!dimensions) {
      env->DeleteLocalRef(data);
      return nullptr;
    }
    std::vector<jint> java_dimensions(
      pixels.dimensions.begin(),
      pixels.dimensions.end());
    env->SetIntArrayRegion(
      dimensions,
      0,
      static_cast<jsize>(java_dimensions.size()),
      java_dimensions.data());

    jstring scalar_type = new_string(env, pixels.scalar_type);
    jstring photometric_interpretation = new_string(env, pixels.photometric_interpretation);
    jstring transfer_syntax = new_string(env, pixels.transfer_syntax);

    jclass pixel_data_class = env->FindClass("com/example/gdcm/PixelData");
    jmethodID constructor = env->GetMethodID(
      pixel_data_class,
      "<init>",
      "([B[IIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    jobject result = env->NewObject(
      pixel_data_class,
      constructor,
      data,
      dimensions,
      static_cast<jint>(pixels.samples_per_pixel),
      static_cast<jint>(pixels.bits_allocated),
      static_cast<jint>(pixels.bits_stored),
      static_cast<jint>(pixels.high_bit),
      static_cast<jint>(pixels.pixel_representation),
      scalar_type,
      photometric_interpretation,
      transfer_syntax);

    env->DeleteLocalRef(data);
    env->DeleteLocalRef(dimensions);
    env->DeleteLocalRef(scalar_type);
    env->DeleteLocalRef(photometric_interpretation);
    env->DeleteLocalRef(transfer_syntax);

    return result;
  } catch (const std::exception& error) {
    throw_runtime_exception(env, error.what());
    return nullptr;
  }
}
