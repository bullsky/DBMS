from django.urls import path

from . import views

app_name = 'vbank'

urlpatterns = [
    path('', views.index, name='index'),
    path('regist/', views.regist, name='regist'),
    path('regist/gonext/', views.regist_next, name='regist_next'),
    path('regist/setting/', views.regist_set, name='regist_set'),
    path('login/', views.login, name='login'),
    path('home/', views.home, name='home'),
    path('balance/', views.balance, name='balance'),
    path('transfer/', views.transfer, name='transfer'),
    path('transfer/gonext/', views.transfer_next, name='transfer_next'),
    path('flow/', views.flow, name='flow'),
    path('logout/', views.logout, name='logout'),
    path('changepass/', views.changepass, name='changepass'),
    path('setnewpass/', views.setnewpass, name='setnewpass'),
    path('unbindcard/', views.unbindcard, name='unbindcard'),
    path('unbind/', views.unbind, name='unbind'),
]
