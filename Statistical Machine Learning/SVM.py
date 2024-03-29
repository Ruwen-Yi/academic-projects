# -*- coding: utf-8 -*-
"""Assignment2.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1y2MO3WN4nkX8NhQ6bwrZMdBgcSLqv20N
"""

pip install cvxpy

# from google.colab import drive
# drive.mount('/content/drive'，force_remount=False)
import pandas as pd
import cvxpy as cp
import numpy as np
from sklearn.metrics import accuracy_score

dataset_train = pd.read_csv('/content/drive/MyDrive/Assignment-2/train.csv', header=None)
dataset_test = pd.read_csv('/content/drive/MyDrive/Assignment-2/test.csv', header=None)

# dataset for training
x_train = dataset_train.iloc[:4000, 1:].values
y_train = dataset_train.iloc[:4000, :1].values
y_train = y_train * 2 - 1

# dataset for validation
x_val = dataset_train.iloc[4000:, 1:].values
y_val = dataset_train.iloc[4000:, :1].values
y_val = y_val * 2 - 1

# dataset for test
x_test = dataset_test.iloc[:, 1:].values
y_test = dataset_test.iloc[:, :1].values
y_test = y_test * 2 - 1

"""# Q2"""

# svm primal model
def svm_train_primal( data_train , label_train , C ):
    m, n = data_train.shape
    w = cp.Variable(shape=(n,1))
    b = cp.Variable()
    slack = cp.Variable(shape=(m,1))

    prob = cp.Problem(cp.Minimize(0.5 * cp.square(cp.norm(w)) + C * cp.sum(slack)), 
                      [ (cp.multiply(label_train, data_train@w + b) - 1 + slack) >= 0, slack >= 0 ])
    prob.solve()

    return (w.value, b.value)

def svm_predict_primal (data_test, label_test, svm_model):
    label_pred = np.sign(data_test @ svm_model[0] + svm_model[1])
    return accuracy_score(label_test, label_pred)

C = 100 / 4000
svm_model = svm_train_primal ( x_train , y_train , C )

# b value
b = svm_model[1]
print("b value : ", b)

#sum of all dimensions of w
w = svm_model[0]
w_sum = np.sum(w)
print("sum of w : ", w_sum)

# prediction accuracy rate
test_accuracy = svm_predict_primal (x_test, y_test, svm_model)
print("accuracy rate : ", test_accuracy)

"""# Q3"""

# svm dual model
def svm_train_dual ( data_train , label_train , C ):
    m, n = data_train.shape
    alpha = cp.Variable(shape=(m,1))

    alpha_y = cp.multiply(alpha, label_train)
    mid_item = data_train @ data_train.T                          # X: N x d

    prob = cp.Problem(cp.Maximize(cp.sum(alpha) - 0.5 * cp.quad_form(alpha_y, mid_item)),
                      [0 <= alpha, alpha <= C, alpha.T@label_train == 0])
    prob.solve()

    return alpha.value

# training in dual form
C = 100 / 4000
svm_model_d = svm_train_dual ( x_train , y_train , C )

# sum of α
alpha = svm_model_d       # (4000, 1)
alpha_sum = np.sum(alpha)
print("sum of alpha : ", alpha_sum)

"""# Q4"""

# w*=sumof(ai*yi*xi) w here is a vector
w_star = x_train.T @ (alpha * y_train)
print("sum of w_star : ", np.sum(w_star))
# print(w_star)

alpha_filter = alpha.copy()

# replace any alpha = 0 with zero
alpha_filter[alpha<1e-05] = 0     
# print(np.count_nonzero(alpha_filter == 0))

# replace any alpha - 0.025 < 1e-05 with zero
alpha_filter[(abs(alpha - 0.025) < 1e-05)] = 0

# concatenate alpha_filter and dataset
# select all the rows with their alpha_filter value not equal to zero
data_temp_set = dataset_train.iloc[:4000, :].values
data_temp_set = np.hstack((data_temp_set, alpha_filter))
data_filter = data_temp_set[data_temp_set[:,201]!=0, :]

# Calculated b : y - x@w
x_filter = data_filter[:, 1:201]
y_filter = data_filter[:, :1]
y_filter = y_filter * 2 - 1

b_set = y_filter - x_filter@w_star
b_star = np.sum(b_set) / y_filter.shape[0]
print("b_star value : ", b_star)

# prediction accuracy rate
test_accuracy = accuracy_score(y_test, np.sign(x_test @ w_star + b_star))
print("accuracy rate : ", test_accuracy)

"""# Q5 Q6
find support vector
"""

# keep one decimal place
# yi(W.T + b) - 1 = 0
y_result_p = np.multiply(y_train, x_train@np.round(w,1) + np.round(b,1)) - 1
print("support vector of primal: ",np.count_nonzero(y_result_p - 0 < 1e-5))

# yi(W*.T + b*) -1 <= 0
y_result_d = np.multiply(y_train, x_train@np.round(w_star,1) + np.round(b_star,1)) - 1
print("support vector of dual: ",np.count_nonzero(y_result_d < 1e-5))

"""# Q7 
find Optimal C
"""

for i in range(-20, 12, 2): # i from -5 to 3 with step 2
    C  = (2**i)/4000
    tempModel = svm_train_primal(x_train, y_train, C)
    accuracy = svm_predict_primal(x_val, y_val, tempModel)
    print("C= (2 ^",i, ")/4000 Accuracy= ", accuracy)

svm_primal_model_C = svm_train_primal(x_train, y_train, C=(2**(2))/4000)
test_accuracy_C = svm_predict_primal(x_test, y_test, svm_primal_model_C)
print("test accuracy: ",test_accuracy_C)

"""# Q8"""

from sklearn.svm import LinearSVC
sklern_svm = LinearSVC(C=2**(2)/4000) # Optimal C from previous result

sklern_svm.fit(x_train, y_train)

label_pred = sklern_svm.predict(x_test)
test_accuracy_sklearn = accuracy_score(y_test,label_pred)
print("test accuracy: ",test_accuracy_sklearn)
