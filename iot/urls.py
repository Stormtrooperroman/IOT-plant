from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('update/', views.update_data, name='update'),
    path('data/', views.data, name='data'),
    path('light/', views.update_led, name='light'),
    path('water/', views.add_water, name='water'),
    path('test/', views.test, name='test'),
]
