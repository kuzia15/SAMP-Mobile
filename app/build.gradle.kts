plugins {
    id("com.android.application")
    id("com.google.gms.google-services")
    id("com.google.firebase.crashlytics")
}

android {
    compileSdk = 36

    namespace = "com.gta.game"

    defaultConfig {
        applicationId = "com.gta.game"
        minSdk = 28
        targetSdk = 36
        versionCode = 1
        versionName = "1.0.0"

        multiDexEnabled = true

        ndk {
            abiFilters.add("arm64-v8a")
        }

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    packaging {
        jniLibs {
            excludes.add("META-INF/*")
            // libbass*.so is built/imported by CMake and also sits in jniLibs
            pickFirsts.add("**/libbass.so")
            pickFirsts.add("**/libbass_fx.so")
            pickFirsts.add("**/libbass_ssl.so")
        }
        resources {
            excludes.add("META-INF/*")
        }
    }

    ndkVersion = "26.2.11394342"

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
        }
    }

    applicationVariants.all {
        outputs.all {
            val apkName = "GTA-2.11-${buildType.name}.apk"
            (this as com.android.build.gradle.internal.api.BaseVariantOutputImpl).outputFileName = apkName
        }
    }

    buildTypes {
        debug {
            isDebuggable = true
            isJniDebuggable = true
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            ndk {
                debugSymbolLevel = "FULL"
            }
        }
        release {
            firebaseCrashlytics {
                nativeSymbolUploadEnabled = true
                strippedNativeLibsDir = "build/intermediates/stripped_native_libs/release/out/lib"
                unstrippedNativeLibsDir = "build/intermediates/merged_native_libs/release/out/lib"
            }

            ndk {
                debugSymbolLevel = "FULL"
            }

            isDebuggable = false
            isJniDebuggable = false
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    lint {
        checkReleaseBuilds = false
        abortOnError = false
    }

    buildFeatures {
        prefab = true
    }
}

dependencies {
    implementation(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.9.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    implementation("androidx.navigation:navigation-fragment:2.7.2")
    implementation("androidx.navigation:navigation-ui:2.7.2")
    implementation("androidx.core:core-ktx:1.12.0")
    implementation("com.google.android.play:asset-delivery:2.3.0")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
    implementation("com.android.volley:volley:1.2.1")
    implementation("com.intuit.sdp:sdp-android:1.1.0")
    implementation("com.google.firebase:firebase-analytics")
    //implementation("com.google.firebase:firebase-crashlytics-ndk")
    implementation(platform("com.google.firebase:firebase-bom:34.13.0"))
    implementation("com.google.firebase:firebase-messaging")
    implementation("com.google.firebase:firebase-config")
    implementation("androidx.datastore:datastore-preferences:1.0.0")
    implementation("androidx.datastore:datastore-core:1.0.0")
    implementation("org.ini4j:ini4j:0.5.4")
    implementation("com.github.bumptech.glide:glide:4.15.1")
    annotationProcessor("com.github.bumptech.glide:compiler:4.15.1")
    implementation("androidx.lifecycle:lifecycle-process:2.6.2")
    implementation("com.joom.paranoid:paranoid-gradle-plugin:0.3.14")
    implementation("com.bytedance.android:shadowhook:1.0.10")
    implementation("androidx.multidex:multidex:2.0.1")
    implementation("com.github.amitshekhariitbhu:PRDownloader:1.0.2")
}