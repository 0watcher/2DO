<div align="center">

# 2DO

***"To do or not to do? That is the question"***

![Work](https://img.shields.io/badge/work-in_progress-yellow.svg)
![Version](https://img.shields.io/badge/version-0.0.1-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-green.svg)

</div>

## About the project

It's just simple todo list with some additional features.

## Dependencies
- gtest
- SQLiteCpp

## Requirements
1. user
- create (name, password, role)
- delete by nickname
- change name
- change password
2. task
- create (topic, content, deadline, task_owner, task_executor)
- delete by id
- change status
- change topic
- change content
- change deadline
- comment task
3. database
- add new user data
- update new user data
- delete new user data
- add new task data
- update task data
- delete task data
4. error
- handle error in a gentle way using Result sim from rust lang
5. app
- main loop





