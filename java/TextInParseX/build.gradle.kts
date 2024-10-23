gradle.startParameter.isOffline = false

plugins {
    id("java")
    id("maven-publish")
    id("signing")
}

java {
    withSourcesJar()
    withJavadocJar()
}

group = "io.github.supperai"
version = "1.0.2"

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
    implementation("org.openpnp:opencv:4.5.5-1")
    testImplementation(platform("org.junit:junit-bom:5.10.0"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    implementation("org.apache.poi:poi:5.2.3")
    implementation("org.apache.poi:poi-ooxml:5.2.3")
}

tasks.test {
    useJUnitPlatform()
}

tasks.javadoc {
    executable = "/usr/bin/javadoc"
    options.encoding = "UTF-8"
    (options as StandardJavadocDocletOptions).addStringOption("Xdoclint:none", "-quiet")
}

// 配置发布
publishing {
    publications {
        create<MavenPublication>("mavenJava") {
            artifactId = "parse_sdk"
            from(components["java"])
            
            // Remove these lines as they are redundant
            // artifact(tasks.jar)
            // artifact(tasks["sourcesJar"])
            // artifact(tasks["javadocJar"])
            
            pom {
                name.set("TextInParseX SDK")
                description.set("A SDK for TextInParseX")
                url.set("https://github.com/supperai/parsex-sdk")
                licenses {
                    license {
                        name.set("The Apache License, Version 2.0")
                        url.set("http://www.apache.org/licenses/LICENSE-2.0.txt")
                    }
                }
                developers {
                    developer {
                        id.set("supperai")
                        name.set("supperai")
                        email.set("xxx@xxx.com")
                    }
                }
                scm {
                    connection.set("scm:git:git://github.com/supperai/parsex-sdk.git")
                    developerConnection.set("scm:git:ssh://github.com/supperai/parsex-sdk.git")
                    url.set("https://github.com/supperai/parsex-sdk")
                }
            }
        }
    }
    repositories {
        maven {
            name = "myRepo"
            url = uri("${layout.buildDirectory.get().asFile}/repo")
        }
    }
}

signing {
    sign(publishing.publications["mavenJava"])
}

tasks.register("generateChecksums") {
    dependsOn("jar", "sourcesJar", "javadocJar")
    doLast {
        fileTree(layout.buildDirectory.dir("libs")).matching {
            include("*.jar")
        }.forEach { file ->
            ant.withGroovyBuilder {
                "checksum"("file" to file, "algorithm" to "MD5", "fileext" to ".md5")
                "checksum"("file" to file, "algorithm" to "SHA1", "fileext" to ".sha1")
            }
        }
    }
}

tasks.register<Zip>("bundleForUpload") {
    dependsOn("publish", "generateChecksums")
}


