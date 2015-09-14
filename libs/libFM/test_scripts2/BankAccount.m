classdef BankAccount < handle
    properties
        name
        balance
    end
    methods
function obj = BankAccount(name, balance)
obj.name = name
obj.balance = balance
end
function deposit(obj,amount)
    obj.balance = obj.balance + amount
end
function withdraw(obj,amount)
    obj.balance = obj.balance - amount
end
function disp(obj)
    ['Name: ' obj.name]
    ['Balance: ' obj.balance]
end
end