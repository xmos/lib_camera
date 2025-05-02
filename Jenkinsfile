@Library('xmos_jenkins_shared_library@v0.38.0') _

def runningOn(machine) {
  println "Stage running on:"
  println machine
}


getApproval()
pipeline {
  agent none
  environment {
    REPO = 'lib_camera'
    REPO_NAME = "lib_camera"
  } // environment
  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.3.1',
      description: 'The XTC tools version'
    )
    string(
      name: 'XMOSDOC_VERSION',
      defaultValue: 'v7.0.0',
      description: 'The xmosdoc version')

    string(
      name: 'INFR_APPS_VERSION',
      defaultValue: 'feature/xcommon_cmake_deps_check', //TODO update to develop
      description: 'The infr_apps version'
    )
  } // parameters
  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=false))
  } // options

  stages{
  stage('CI') {
    parallel {
      stage ('Build & Test') {
        agent {label 'xcore.ai'}
        stages {
          stage('Checkout') {
            steps {
              runningOn(env.NODE_NAME)
              dir(REPO)
              {
                checkoutScmShallow()
                createVenv(reqFile: "requirements.txt")
              }
            } // steps
          } // Checkout

          stage('Examples build') {
            steps{
              dir("${REPO}/examples") {
                withVenv {
                  xcoreBuild()
                }
              }
            }
          } // Examples build

          stage('Tests build') {
            steps{
              dir("${REPO}/tests") {
                withVenv {
                  xcoreBuild()
                }
              }
            }
          } // Tests build

          stage("Lib checks"){ // Needs to be placed after build stage for dependancies to be built
            steps {
              dir("${REPO}") {
                withVenv {
                  runLibraryChecks("${WORKSPACE}/${REPO}", "${params.INFR_APPS_VERSION}")
                }
              }
            }
          } // Lib checks

          stage('Unit tests') {
            steps {
              dir("${REPO}/tests/unit_tests") {
                withTools(params.TOOLS_VERSION) {
                  sh 'xrun --id 0 --xscope bin/unit_tests.xe'
                }
              }
            }
          } // Unit tests

          stage('ISP tests') {
            steps {
              dir('lib_camera/tests/isp') {
                withVenv {
                  withTools(params.TOOLS_VERSION) {
                    sh 'pytest -n auto'
                  } // withTools
                  archiveArtifacts artifacts: "test_results.csv"
                  archiveArtifacts artifacts: "imgs/images.zip"
                }
              }
            }
          } // ISP tests
        } // stages
      } // Build & Test
      
      stage('Documentation') {
        agent {label 'documentation'}
        steps{
            runningOn(env.NODE_NAME)
            dir("${REPO}") {
            checkoutScmShallow()
            createVenv(reqFile: "requirements.txt")
            withVenv {
              buildDocs()
            }
          }
        }
      } // Documentation
    } // parallel
  } // CI
  } // stages
  


  post {
    cleanup {
      cleanWs()
    }
  } // post

} // pipeline
