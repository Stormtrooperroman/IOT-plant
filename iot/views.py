from django.shortcuts import render
from django.http import HttpResponse
import json
import requests

from django.utils import timezone
from .models import Information

def index(request):
    info = {'data' : get_data()}
    return render(request, 'iot/index.html', info)

def data(request):
    latest_info_list = Information.objects.order_by('-get_date')[:10]
    info = {
        'lastest_info_list': latest_info_list
    }
    return render(request, 'iot/data.html', info)

def test(request):
    return render(request, 'iot/test.html')

def update_data(request):
    info = {'data' : get_data()}
    return HttpResponse(json.dumps(info), content_type='application/json')


def update_led(request):
    info = {'data': light()}
    return HttpResponse(json.dumps(info), content_type='application/json')

def add_water(request):
    info = {'data': water()}
    return HttpResponse(json.dumps(info), content_type='application/json')

def get_data():
    try:
        data = requests.get('http://192.168.2.132/data').json()
        time = timezone.now()
        latest_date = Information.objects.order_by('-get_date')[0]
        if (time - latest_date.get_date).days >= 1 :
            i = Information(air_temp = data["air_temp"], air_hum = data["air_hum"],
                            air_pres = data["air_pres"], light = data["light"], get_date = time)
            i.save()
    except requests.exceptions.Timeout:
        data = {'error': 'Timeout error'}
    except requests.exceptions.TooManyRedirects:
        data = {'error': 'Too many redirects'}
    except requests.exceptions.RequestException :
        data = {'error': 'Unknown error' }
    return data

def light():
    try:
        data = requests.get('http://192.168.2.132/light').json()
    except requests.exceptions.Timeout:
        data = {'error': 'Timeout error'}
    except requests.exceptions.TooManyRedirects:
        data = {'error': 'Too many redirects'}
    except requests.exceptions.RequestException :
        data = {'error': 'Unknown error' }
    return data

def water():
    try:
        data = requests.get('http://192.168.2.132/water').json()
    except requests.exceptions.Timeout:
        data = {'error': 'Timeout error'}
    except requests.exceptions.TooManyRedirects:
        data = {'error': 'Too many redirects'}
    except requests.exceptions.RequestException:
        data = {'error': 'Unknown error' }
    return data
