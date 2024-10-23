gradle.startParameter.isOffline = false

plugins {
    id("java")
    id("maven-publish")
}

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

group = "com.textinparsex"
version = "1.0"

repositories {
    maven { 
        url = uri("https://maven.aliyun.com/repository/public")
    }

    mavenCentral()
}

dependencies {
    implementation("org.apache.httpcomponents:httpclient:4.5.14")
    implementation("com.fasterxml.jackson.core:jackson-databind:2.15.3")
    implementation("org.projectlombok:lombok:1.18.20")
    annotationProcessor("org.projectlombok:lombok:1.18.20")
    implementation("org.apache.logging.log4j:log4j-core:2.20.0")
    implementation("org.openpnp:opencv:4.5.5-1")
    testImplementation(platform("org.junit:junit-bom:5.10.0"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    implementation("org.apache.poi:poi:5.2.3")
    implementation("org.apache.poi:poi-ooxml:5.2.3")
}

tasks.test {
    useJUnitPlatform()
}

// 配置 JAR 任务
tasks.jar {
    archiveBaseName.set("textinparsex-sdk")
    archiveVersion.set("1.0")
    
    manifest {
        attributes(mapOf(
            "Implementation-Title" to project.name,
            "Implementation-Version" to project.version
        ))
    }
    // 如果您有源代码，可以将其包含在 JAR 中
    from(sourceSets.main.get().allSource)
}

// 配置发布
publishing {
    publications {
        create<MavenPublication>("sdk") {
            groupId = "com.textinparsex"
            artifactId = "textinparsex-sdk"
            version = "1.0"
            
            from(components["java"])
        }
    }
}
