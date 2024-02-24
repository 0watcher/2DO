## TODO
- add size_t when possible and improve some for loops
- add const when possible
- improve menu class
- fix backing to users menu after user deletion
- add menu_print in IPrinter
- make user logs
- finish app

## UI SCHEMA

First time:
New user:

Main:
Your current tasks:
SomeTask [DONE]
SomeOtherTask [IN_PROGRESS]

Menu:
 [1] Tasks
 [2] Settings
 [3] Exit
 ->

------------------------

[1]Tasks:
 [1] YourTasks
 [2] DelegatedTasks
 [3] DelegateTasks
 [4] Back
 ->
 
[1]YourTasks:
 Your tasks:
 [1] SomeTask [DONE]
 [2] SomeOtherTask [IN_PROGRESS]

 Select task:
 [[0] Back]->
 
After selecting a task:
 Topic: SomeTask
 Content: This task is super.
 Start date: 0.0.0
 Deadline: 0.0.0
 Status: IN_PROGRESS
 
 [1] Change status
 [2] Discussion
 [3] Back
 ->

[1]Change status
 [1] Done
 [2] In progress
 [3] Rejected

[2]Dicussion:
 SomeUser: It is good.
 SomeOtherUser: It is not good.
 [[0] Back]->

[2]DelegatedTasks
 Delegated tasks:
 [1] SomeTask (to SomeUser) [DONE]
 [2] SomeOtherTask (to SomeUser) [IN_PROGRESS]
 ->

After selecting a task:
 Topic: SomeTask
 Content: This task is super.
 Start date: 0.0.0
 Deadline: 0.0.0
 Executor: SomeUser, AnotherUser
 Owner: SomeOtherUser
 Status: IN_PROGRESS
 
 [1] Change status
 [2] Discussion
 [3] Edit
 [4] Back
 ->

[1]Change status
 [1] Done
 [2] In progress
 [3] Rejected

[2]Dicussion:
 SomeUser: It is good.
 SomeOtherUser: It is not good.
 [[0] Back]->

[3]Edit
 [1] Edit topic
 [2] Edit content
 [3] Edit deadline
 [4] Edit executors
 [4] Back	

[3]Delegate task
 Create new task:
 Topic:
 Content:
 Start date:
 Deadline:
 Executors:

------------------------

Settings:
 [1] ManageUsers
 [2] Advanced
 [3] Back
 ->

[1]ManageUsers
 Select user to manage:
 [1] SomeUser
 [2] SomeOtherUser
 [[0] Back]->

After selecting a user:
 [1] Change role
 [2] Delete user
 [3] Back 
 ->

[2]Advanced
 [1] Wipe workspace data

------------------------